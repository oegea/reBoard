#include <gtest/gtest.h>

#include <stdlib.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "infrastructure/packages/FilePackageRepository.h"
#include "infrastructure/packages/PackageTemplating.h"
#include "infrastructure/system/DeviceSlug.h"

using namespace reboard;
namespace fs = std::filesystem;

namespace {

class TempDirectory {
public:
    TempDirectory() {
        std::string templatePath = (fs::temp_directory_path() / "reboard-pkg-XXXXXX").string();
        std::vector<char> buffer(templatePath.begin(), templatePath.end());
        buffer.push_back('\0');
        if (::mkdtemp(buffer.data()) == nullptr) {
            throw std::runtime_error("mkdtemp failed");
        }
        path_ = buffer.data();
    }
    ~TempDirectory() {
        std::error_code errorCode;
        fs::remove_all(path_, errorCode);
    }
    const std::string& path() const { return path_; }

private:
    std::string path_;
};

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

// Builds a valid package tar.gz with a manifest and a payload file.
std::string buildFixturePackage(const TempDirectory& workspace) {
    const std::string stage = workspace.path() + "/stage";
    fs::create_directories(stage);
    std::ofstream(stage + "/manifest.app")
        << "name=Demo\nexec={APP_DIR}/run.sh\nicon={APP_DIR}/icon.png\n";
    std::ofstream(stage + "/run.sh") << "#!/bin/sh\nexit 0\n";
    const std::string package = workspace.path() + "/demo.tar.gz";
    const std::string command = "tar -czf '" + package + "' -C '" + stage + "' .";
    if (std::system(command.c_str()) != 0) {
        throw std::runtime_error("failed to build the fixture package");
    }
    return package;
}

}  // namespace

TEST(PackageTemplatingTest, ExpandsEveryToken) {
    EXPECT_EQ(infrastructure::expandAppDirToken("exec={APP_DIR}/a\nicon={APP_DIR}/i\n", "/opt/x"),
              "exec=/opt/x/a\nicon=/opt/x/i\n");
}

TEST(PackageTemplatingTest, LeavesPlainContentUntouched) {
    EXPECT_EQ(infrastructure::expandAppDirToken("name=A\n", "/opt/x"), "name=A\n");
}

TEST(DeviceSlugTest, MapsKnownMachines) {
    EXPECT_EQ(infrastructure::deviceSlugFromMachineName("reMarkable 2.0"), "rm2");
    EXPECT_EQ(infrastructure::deviceSlugFromMachineName("reMarkable 1.0"), "rm1");
    EXPECT_EQ(infrastructure::deviceSlugFromMachineName("reMarkable Ferrari"), "ferrari");
    EXPECT_EQ(infrastructure::deviceSlugFromMachineName("something new"), "rm2");
}

TEST(FilePackageRepositoryTest, InstallsAndRegistersTheManifest) {
    TempDirectory workspace;
    const std::string package = buildFixturePackage(workspace);
    const std::string appsRoot = workspace.path() + "/apps";
    const std::string manifests = workspace.path() + "/apps-store";
    infrastructure::FilePackageRepository repository(appsRoot, manifests);
    const domain::ApplicationId id("demo");

    repository.install(id, package, "1.2.3");

    EXPECT_TRUE(repository.isInstalled(id));
    ASSERT_TRUE(repository.installedVersion(id));
    EXPECT_EQ(*repository.installedVersion(id), "1.2.3");
    EXPECT_TRUE(fs::exists(appsRoot + "/demo/run.sh"));
    const std::string manifest = readFile(manifests + "/demo.app");
    EXPECT_NE(manifest.find("exec=" + appsRoot + "/demo/run.sh"), std::string::npos);
    EXPECT_EQ(manifest.find("{APP_DIR}"), std::string::npos);
}

TEST(FilePackageRepositoryTest, UninstallRemovesEverything) {
    TempDirectory workspace;
    const std::string package = buildFixturePackage(workspace);
    const std::string appsRoot = workspace.path() + "/apps";
    const std::string manifests = workspace.path() + "/apps-store";
    infrastructure::FilePackageRepository repository(appsRoot, manifests);
    const domain::ApplicationId id("demo");
    repository.install(id, package, "1.0.0");

    repository.uninstall(id);

    EXPECT_FALSE(repository.isInstalled(id));
    EXPECT_FALSE(repository.installedVersion(id));
    EXPECT_FALSE(fs::exists(appsRoot + "/demo"));
    EXPECT_FALSE(fs::exists(manifests + "/demo.app"));
}

TEST(FilePackageRepositoryTest, RejectsPackagesWithoutManifest) {
    TempDirectory workspace;
    const std::string stage = workspace.path() + "/stage";
    fs::create_directories(stage);
    std::ofstream(stage + "/whatever.txt") << "no manifest here\n";
    const std::string package = workspace.path() + "/bad.tar.gz";
    ASSERT_EQ(std::system(("tar -czf '" + package + "' -C '" + stage + "' .").c_str()), 0);

    infrastructure::FilePackageRepository repository(workspace.path() + "/apps",
                                                     workspace.path() + "/apps-store");
    EXPECT_THROW(repository.install(domain::ApplicationId("bad"), package, "1.0.0"),
                 std::runtime_error);
    EXPECT_FALSE(repository.isInstalled(domain::ApplicationId("bad")));
}

TEST(FilePackageRepositoryTest, FailedExtractionThrows) {
    TempDirectory workspace;
    const std::string notATarball = workspace.path() + "/garbage.tar.gz";
    std::ofstream(notATarball) << "garbage";

    infrastructure::FilePackageRepository repository(workspace.path() + "/apps",
                                                     workspace.path() + "/apps-store");
    EXPECT_THROW(repository.install(domain::ApplicationId("garbage"), notATarball, "1.0.0"),
                 std::runtime_error);
}
