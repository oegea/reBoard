# reBoard task runner — the standard entry point for every day-to-day
# operation, similar to npm scripts. Run `make help` to list the tasks.

BUILD_DIR      ?= build
JOBS           ?= $(shell nproc)
CMAKE          ?= cmake
CTEST          ?= ctest

# Official reMarkable SDK (public downloads).
SDK_VERSION       ?= 3.27.0.97
SDK_IMAGE_VERSION ?= 5.7.119
SDK_BASE_URL      ?= https://storage.googleapis.com/remarkable-codex-toolchain
SDK_DIR           ?= sdk
SDK_CACHE         ?= .sdk-cache

# Device deployment (USB connection exposes this address by default).
RM_HOST ?= root@10.11.99.1

.PHONY: help deps test coverage build clean deploy
.PHONY: sdk-rm1 sdk-rm2 sdk-ferrari cross-rm1 cross-rm2 cross-ferrari

help: ## List the available tasks
	@grep -E '^[a-zA-Z0-9_%-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[1m%-16s\033[0m %s\n", $$1, $$2}'

deps: ## Install host dependencies (cmake, gcovr) into ~/.local when missing
	@command -v python3 >/dev/null 2>&1 || { echo "error: python3 is required (install it with your distro package manager)"; exit 1; }
	@python3 -m pip --version >/dev/null 2>&1 || python3 -m ensurepip --user
	@command -v $(CMAKE) >/dev/null 2>&1 || python3 -m pip install --user cmake
	@command -v gcovr >/dev/null 2>&1 || python3 -m pip install --user gcovr
	@command -v g++ >/dev/null 2>&1 || { echo "error: g++ is required (install it with your distro package manager)"; exit 1; }
	@echo "All host dependencies are available. Ensure ~/.local/bin is in your PATH."

test: ## Configure a host build and run the unit tests
	$(CMAKE) -S . -B $(BUILD_DIR) -DREBOARD_BUILD_UI=OFF -DREBOARD_BUILD_TESTS=ON
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

coverage: ## Run the unit tests with coverage instrumentation and print a gcovr report
	$(CMAKE) -S . -B $(BUILD_DIR)-coverage -DREBOARD_BUILD_UI=OFF -DREBOARD_BUILD_TESTS=ON -DREBOARD_COVERAGE=ON
	$(CMAKE) --build $(BUILD_DIR)-coverage -j $(JOBS)
	$(CTEST) --test-dir $(BUILD_DIR)-coverage --output-on-failure
	gcovr --root . --filter 'src/' --exclude 'tests/' --print-summary

build: ## Host build of the business core (UI needs Qt 6: use cross-<device> for devices)
	$(CMAKE) -S . -B $(BUILD_DIR) -DREBOARD_BUILD_UI=OFF -DREBOARD_BUILD_TESTS=OFF
	$(CMAKE) --build $(BUILD_DIR) -j $(JOBS)

sdk-rm1 sdk-rm2 sdk-ferrari: sdk-%: ## Download and install the official reMarkable SDK for a device (rm1, rm2, ferrari)
	@mkdir -p $(SDK_CACHE)
	@test -f $(SDK_CACHE)/$*-toolchain.sh || \
		curl -L --fail -o $(SDK_CACHE)/$*-toolchain.sh \
		$(SDK_BASE_URL)/$(SDK_VERSION)/$*/remarkable-production-image-$(SDK_IMAGE_VERSION)-$*-public-x86_64-toolchain.sh
	@chmod +x $(SDK_CACHE)/$*-toolchain.sh
	@test -d $(SDK_DIR)/$* || $(SDK_CACHE)/$*-toolchain.sh -y -d $(SDK_DIR)/$*
	@echo "SDK for $* installed under $(SDK_DIR)/$*"

cross-rm1 cross-rm2 cross-ferrari: cross-%: sdk-% ## Cross-compile the full launcher for a device (rm1, rm2, ferrari)
	bash -c 'source $(SDK_DIR)/$*/environment-setup-* && \
		$(CMAKE) -S . -B $(BUILD_DIR)-$* -DREBOARD_BUILD_UI=ON -DREBOARD_BUILD_TESTS=OFF && \
		$(CMAKE) --build $(BUILD_DIR)-$* -j $(JOBS)'
	@echo "Binary ready at $(BUILD_DIR)-$*/src/reboard"

deploy: ## Copy the reMarkable 2 binaries and packaging files to the device (RM_HOST=root@10.11.99.1)
	scp $(BUILD_DIR)-rm2/src/reboard $(BUILD_DIR)-rm2/src/reboard-ui $(RM_HOST):/home/root/
	scp packaging/reboard.service $(RM_HOST):/etc/systemd/system/reboard.service
	@echo "Deployed. On the device: systemctl daemon-reload && systemctl start reboard"

clean: ## Remove every build directory
	rm -rf $(BUILD_DIR) $(BUILD_DIR)-* build-*
