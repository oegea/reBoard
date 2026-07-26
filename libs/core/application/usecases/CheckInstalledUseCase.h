#pragma once

#include "domain/repositories/PackageRepository.h"
#include "domain/valueobjects/ApplicationId.h"

namespace reboard::application {

// Tells whether a store application is currently installed.
class CheckInstalledUseCase {
public:
    explicit CheckInstalledUseCase(const domain::PackageRepository& packageRepository)
        : packageRepository_(packageRepository) {}

    bool execute(const domain::ApplicationId& id) const {
        return packageRepository_.isInstalled(id);
    }

    std::optional<std::string> installedVersion(const domain::ApplicationId& id) const {
        return packageRepository_.installedVersion(id);
    }

private:
    const domain::PackageRepository& packageRepository_;
};

}  // namespace reboard::application
