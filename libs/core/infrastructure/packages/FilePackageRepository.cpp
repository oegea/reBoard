#include "infrastructure/packages/FilePackageRepository.h"

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <sys/wait.h>
#include <unistd.h>

#include "infrastructure/packages/PackageTemplating.h"

namespace reboard::infrastructure {

namespace fs = std::filesystem;

FilePackageRepository::FilePackageRepository(std::string appsRoot,
                                             std::string storeManifestDirectory,
                                             std::string tarBinary)
    : appsRoot_(std::move(appsRoot)),
      storeManifestDirectory_(std::move(storeManifestDirectory)),
      tarBinary_(std::move(tarBinary)) {}

std::string FilePackageRepository::appDirectory(const domain::ApplicationId& id) const {
    return appsRoot_ + "/" + id.value();
}

std::string FilePackageRepository::manifestPath(const domain::ApplicationId& id) const {
    return storeManifestDirectory_ + "/" + id.value() + ".app";
}

void FilePackageRepository::install(const domain::ApplicationId& id,
                                    const std::string& packagePath) {
    const std::string destination = appDirectory(id);

    std::error_code errorCode;
    fs::remove_all(destination, errorCode);  // Reinstall == replace.
    fs::create_directories(destination);
    fs::create_directories(storeManifestDirectory_);

    if (runTar(packagePath, destination) != 0) {
        fs::remove_all(destination, errorCode);
        throw std::runtime_error("Package extraction failed for " + id.value());
    }

    const std::string packagedManifest = destination + "/manifest.app";
    std::ifstream manifestFile(packagedManifest);
    if (!manifestFile) {
        fs::remove_all(destination, errorCode);
        throw std::runtime_error("Package for " + id.value() + " has no manifest.app");
    }
    std::ostringstream content;
    content << manifestFile.rdbuf();

    std::ofstream registered(manifestPath(id));
    if (!registered) {
        fs::remove_all(destination, errorCode);
        throw std::runtime_error("Cannot register the manifest for " + id.value());
    }
    registered << expandAppDirToken(content.str(), destination);
}

void FilePackageRepository::uninstall(const domain::ApplicationId& id) {
    std::error_code errorCode;
    fs::remove(manifestPath(id), errorCode);
    fs::remove_all(appDirectory(id), errorCode);
}

bool FilePackageRepository::isInstalled(const domain::ApplicationId& id) const {
    std::error_code errorCode;
    return fs::exists(manifestPath(id), errorCode);
}

int FilePackageRepository::runTar(const std::string& packagePath,
                                  const std::string& destination) const {
    const pid_t pid = ::fork();
    if (pid < 0) {
        throw std::runtime_error(std::string("fork failed: ") + std::strerror(errno));
    }
    if (pid == 0) {
        const char* argv[] = {tarBinary_.c_str(), "-xzf", packagePath.c_str(),
                              "-C",               destination.c_str(), nullptr};
        ::execvp(tarBinary_.c_str(), const_cast<char* const*>(argv));
        ::_exit(127);
    }
    int status = 0;
    pid_t waited;
    do {
        waited = ::waitpid(pid, &status, 0);
    } while (waited < 0 && errno == EINTR);
    if (waited != pid || !WIFEXITED(status)) {
        return -1;
    }
    return WEXITSTATUS(status);
}

}  // namespace reboard::infrastructure
