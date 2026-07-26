#pragma once

#include "domain/repositories/PackageRepository.h"
#include "domain/valueobjects/ApplicationId.h"

namespace reboard::application {

// Removes a store-installed application (manifest + files). The package
// repository only ever touches the store-managed locations, so base system
// applications cannot be uninstalled by construction (ADR-0006).
class UninstallApplicationUseCase {
public:
    explicit UninstallApplicationUseCase(domain::PackageRepository& packageRepository)
        : packageRepository_(packageRepository) {}

    void execute(const domain::ApplicationId& id) { packageRepository_.uninstall(id); }

private:
    domain::PackageRepository& packageRepository_;
};

}  // namespace reboard::application
