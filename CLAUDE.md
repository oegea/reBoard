# CLAUDE.md

Guidance for Claude Code when working in this repository. The project
requirements live in [docs/PROJECT.md](docs/PROJECT.md) — read them first.

## What reBoard is

A springboard-inspired application launcher for reMarkable paper tablets:
paginated app grid + fixed bottom dock, resident in memory, with an always-on
"go home" touch gesture. Nothing is embedded in the binary; applications are
external binaries or systemd units declared through manifest files.

## Common commands

The root `Makefile` is the task runner (like npm scripts):

```sh
make deps        # install host deps (cmake, gcovr) into ~/.local (needs ~/.local/bin in PATH)
make test        # host build + unit tests (ctest)
make coverage    # tests with coverage + gcovr summary
make sdk-rm2     # download/install the official SDK (also sdk-rm1, sdk-ferrari)
make cross-rm2   # cross-compile the full launcher for reMarkable 2
make deploy      # scp binary + service to RM_HOST (default root@10.11.99.1)
make clean
```

Run a single test: `./build/tests/reboard_tests --gtest_filter='BoardTest.*'`

## Architecture rules (from PROJECT.md — do not violate)

- `src/domain/` depends on **nothing**. Immutable value objects and entities
  (throwing constructors validate invariants), the pure `GestureDetector`
  service, and repository interfaces.
- `src/infrastructure/` implements domain repository interfaces only.
- `src/application/` use cases depend **only on domain**; the single
  exception is `UseCaseFactory`, which wires infrastructure implementations.
- `src/ui/` (Qt) calls use cases; no business logic in the UI.
- Everything (code, comments, docs) is in English.

## Key design decisions

- **Official SDK path**: the UI is Qt Quick rendered by the official `epaper`
  Qt platform plugin (`-platform epaper`, `QT_QUICK_BACKEND=epaper`). Built
  with the official Yocto toolchains, downloaded publicly from
  `storage.googleapis.com/remarkable-codex-toolchain/<version>/<device>/...`
  (see the Makefile `sdk-%` target). Devices: `rm1`, `rm2`, `ferrari`
  (Paper Pro).
- **Core is Qt-free**: `reboard_core` (domain + application + infrastructure)
  is plain C++17 so unit tests build/run on any Linux host without Qt
  (`-DREBOARD_BUILD_UI=OFF`).
- **App manifests**: `<id>.app` key=value files (`name`, `exec` XOR `unit`,
  `icon`, `dock`) in `/etc/reboard/apps`, `/opt/etc/reboard/apps`,
  `~/.config/reboard/apps`. Id = file stem. User manifests override built-ins
  (CompositeApplicationRepository: first source wins).
- **Built-in entry**: "Notebooks" → systemd unit `xochitl`, pinned to dock.
  Returning to the stock UI = `systemctl start xochitl`.
- **Foreground tracking**: `SessionRepository` (in-memory, mutex-protected)
  holds the current foreground app. On startup
  `AdoptRunningApplicationUseCase` adopts an already-running unit app (e.g.
  xochitl) so the home gesture can close it. `RefreshForegroundStateUseCase`
  is polled every 2 s by the UI to notice apps that exit on their own.
- **Process control**: direct children are fork/exec'd with `setsid()`; stop
  = SIGTERM to the process group with a fallback to the pid (races). Children
  are reaped with `waitpid(WNOHANG)` inside `isRunning()`. systemd units go
  through `systemctl` (path injectable for tests — never call real systemctl
  in tests, it triggers polkit password prompts on developer machines).
- **Touch input**: `EvdevTouchScreen` auto-detects the digitizer (first
  device with `ABS_MT_POSITION_X`), tracks multitouch slot 0, normalizes to
  [0,1] and stamps samples with the monotonic clock. On reMarkable 2 the Y
  axis is inverted relative to the screen (default `invertY=true`, overridable
  via `REBOARD_TOUCH_*` env vars).
- **Gestures**: pure `GestureDetector` (domain): swipe up from bottom edge
  (start y ≥ 0.92, travel ≥ 0.20, ≤ 700 ms) or long press (≥ 1.5 s, movement
  ≤ 0.03). Long press needs the 100 ms tick (`poll`) from the evdev loop.
- **Tests**: GoogleTest via FetchContent. Use cases are tested with hand
  written fakes; `PosixProcessRepository` with real `/bin/sleep`, `/bin/true`
  children and `/bin/true|/bin/false` as fake systemctl.

## CI

`.github/workflows/ci.yml`: pushes to `main` run host unit tests + coverage,
then cross-compile for `rm1`, `rm2`, `ferrari` with the official SDK (installer
cached) and upload `reboard-<device>` artifacts. PRs only run the test job.

## Gotchas

- The host may lack `cmake`/`pip3`: `make deps` bootstraps pip via
  `python3 -m ensurepip --user` and installs into `~/.local`.
- Qt is only found when cross-compiling (SDK sysroot). Host builds must use
  `-DREBOARD_BUILD_UI=OFF`.
- QML changes require no extra tooling beyond `rcc` (resources.qrc is
  compiled by `qt6_add_resources`).
- On device, reBoard must own the display: stop `xochitl` first (the
  `reboard.service` unit does it via `ExecStartPre`).
