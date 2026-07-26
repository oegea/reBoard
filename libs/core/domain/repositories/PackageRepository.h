#pragma once

#include <optional>
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
    // picks the application up. `version` is recorded so updates can be
    // detected later. Throws std::runtime_error on failure.
    virtual void install(const ApplicationId& id, const std::string& packagePath,
                         const std::string& version) = 0;

    // Removes the store manifest and the application files.
    virtual void uninstall(const ApplicationId& id) = 0;

    virtual bool isInstalled(const ApplicationId& id) const = 0;

    // Version recorded at install time; empty when not installed.
    virtual std::optional<std::string> installedVersion(const ApplicationId& id) const = 0;
};

}  // namespace reboard::domain
