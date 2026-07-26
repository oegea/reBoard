#pragma once

#include <string>
#include <vector>

#include <cstdlib>

#include "application/usecases/AdoptRunningApplicationUseCase.h"
#include "application/usecases/CheckInstalledUseCase.h"
#include "application/usecases/CloseForegroundApplicationUseCase.h"
#include "application/usecases/GetBoardUseCase.h"
#include "application/usecases/InstallPackageUseCase.h"
#include "application/usecases/LaunchApplicationUseCase.h"
#include "application/usecases/RefreshForegroundStateUseCase.h"
#include "application/usecases/UninstallApplicationUseCase.h"
#include "infrastructure/packages/FilePackageRepository.h"
#include "infrastructure/processes/PosixProcessRepository.h"
#include "infrastructure/repositories/BuiltInApplicationRepository.h"
#include "infrastructure/repositories/CompositeApplicationRepository.h"
#include "infrastructure/repositories/FileApplicationRepository.h"
#include "infrastructure/repositories/InMemorySessionRepository.h"

namespace reboard::application {

// Composition root of the business layer: wires the use cases with the
// infrastructure implementations of the domain repositories. This is the only
// place in the application layer allowed to depend on infrastructure.
class UseCaseFactory {
public:
    // Default store locations (ADR-0006), overridable for tests.
    static std::string defaultStoreAppsRoot() {
        return homeDirectory() + "/.local/share/reboard/apps";
    }
    static std::string defaultStoreManifestDirectory() {
        return homeDirectory() + "/.config/reboard/apps-store";
    }

    explicit UseCaseFactory(std::vector<std::string> manifestDirectories,
                            std::string storeAppsRoot = defaultStoreAppsRoot(),
                            std::string storeManifestDirectory = defaultStoreManifestDirectory())
        : fileApplications_(std::move(manifestDirectories), {storeManifestDirectory}),
          packages_(std::move(storeAppsRoot), std::move(storeManifestDirectory)),
          applications_({&fileApplications_, &builtInApplications_}),
          getBoard_(applications_),
          launchApplication_(applications_, processes_, session_),
          closeForegroundApplication_(processes_, session_),
          adoptRunningApplication_(applications_, processes_, session_),
          refreshForegroundState_(processes_, session_),
          installPackage_(packages_),
          uninstallApplication_(packages_),
          checkInstalled_(packages_) {}

    GetBoardUseCase& getBoard() { return getBoard_; }
    LaunchApplicationUseCase& launchApplication() { return launchApplication_; }
    CloseForegroundApplicationUseCase& closeForegroundApplication() {
        return closeForegroundApplication_;
    }
    AdoptRunningApplicationUseCase& adoptRunningApplication() { return adoptRunningApplication_; }
    RefreshForegroundStateUseCase& refreshForegroundState() { return refreshForegroundState_; }
    InstallPackageUseCase& installPackage() { return installPackage_; }
    UninstallApplicationUseCase& uninstallApplication() { return uninstallApplication_; }
    CheckInstalledUseCase& checkInstalled() { return checkInstalled_; }

private:
    static std::string homeDirectory() {
        const char* home = std::getenv("HOME");
        return home != nullptr ? home : "/home/root";
    }

    infrastructure::FileApplicationRepository fileApplications_;
    infrastructure::FilePackageRepository packages_;
    infrastructure::BuiltInApplicationRepository builtInApplications_;
    infrastructure::CompositeApplicationRepository applications_;
    infrastructure::PosixProcessRepository processes_;
    infrastructure::InMemorySessionRepository session_;

    GetBoardUseCase getBoard_;
    LaunchApplicationUseCase launchApplication_;
    CloseForegroundApplicationUseCase closeForegroundApplication_;
    AdoptRunningApplicationUseCase adoptRunningApplication_;
    RefreshForegroundStateUseCase refreshForegroundState_;
    InstallPackageUseCase installPackage_;
    UninstallApplicationUseCase uninstallApplication_;
    CheckInstalledUseCase checkInstalled_;
};

}  // namespace reboard::application
