#pragma once

#include <string>

#include "domain/repositories/PackageRepository.h"
#include "domain/valueobjects/ApplicationId.h"

namespace reboard::application {

// Installs a downloaded store package so it appears on the board.
class InstallPackageUseCase {
public:
    explicit InstallPackageUseCase(domain::PackageRepository& packageRepository)
        : packageRepository_(packageRepository) {}

    void execute(const domain::ApplicationId& id, const std::string& packagePath,
                 const std::string& version) {
        packageRepository_.install(id, packagePath, version);
    }

private:
    domain::PackageRepository& packageRepository_;
};

}  // namespace reboard::application
