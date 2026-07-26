#pragma once

#include <string>

#include "domain/repositories/PackageRepository.h"

namespace reboard::infrastructure {

// Store package management on disk (ADR-0006):
// - packages extract to `<appsRoot>/<id>/`
// - their manifest (with {APP_DIR} expanded) is registered as
//   `<storeManifestDirectory>/<id>.app`
// Only manifests inside the store manifest directory are ever removable.
class FilePackageRepository : public domain::PackageRepository {
public:
    FilePackageRepository(std::string appsRoot, std::string storeManifestDirectory,
                          std::string tarBinary = "tar");

    void install(const domain::ApplicationId& id, const std::string& packagePath,
                 const std::string& version) override;
    void uninstall(const domain::ApplicationId& id) override;
    bool isInstalled(const domain::ApplicationId& id) const override;
    std::optional<std::string> installedVersion(const domain::ApplicationId& id) const override;

private:
    std::string appDirectory(const domain::ApplicationId& id) const;
    std::string manifestPath(const domain::ApplicationId& id) const;
    std::string versionPath(const domain::ApplicationId& id) const;
    int runTar(const std::string& packagePath, const std::string& destination) const;

    std::string appsRoot_;
    std::string storeManifestDirectory_;
    std::string tarBinary_;
};

}  // namespace reboard::infrastructure
