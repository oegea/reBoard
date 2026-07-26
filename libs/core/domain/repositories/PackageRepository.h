#pragma once

#include <string>

#include "domain/valueobjects/ApplicationId.h"

namespace reboard::domain {

// Manages store-installed application packages. Implemented by the
// infrastructure layer (tar.gz extraction + manifest management). Only
// store-managed applications can ever be uninstalled (ADR-0006).
class PackageRepository {
public:
    virtual ~PackageRepository() = default;

    // Extracts the package archive and registers its manifest so the board
    // picks the application up. Throws std::runtime_error on failure.
    virtual void install(const ApplicationId& id, const std::string& packagePath) = 0;

    // Removes the store manifest and the application files.
    virtual void uninstall(const ApplicationId& id) = 0;

    virtual bool isInstalled(const ApplicationId& id) const = 0;
};

}  // namespace reboard::domain
