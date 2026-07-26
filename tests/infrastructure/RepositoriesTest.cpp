#include <gtest/gtest.h>

#include <stdlib.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "infrastructure/repositories/BuiltInApplicationRepository.h"
#include "infrastructure/repositories/CompositeApplicationRepository.h"
#include "infrastructure/repositories/FileApplicationRepository.h"
#include "infrastructure/repositories/InMemorySessionRepository.h"

using namespace reboard;
namespace fs = std::filesystem;

namespace {

class TempDirectory {
public:
    TempDirectory() {
        std::string templatePath = (fs::temp_directory_path() / "reboard-test-XXXXXX").string();
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

    void writeFile(const std::string& fileName, const std::string& content) const {
        std::ofstream file(fs::path(path_) / fileName);
        file << content;
    }

private:
    std::string path_;
};

class StaticApplicationRepository : public domain::ApplicationRepository {
public:
    explicit StaticApplicationRepository(std::vector<domain::Application> applications)
        : applications_(std::move(applications)) {}

    std::vector<domain::Application> findAll() const override { return applications_; }

private:
    std::vector<domain::Application> applications_;
};

domain::Application makeApp(const std::string& id, const std::string& name) {
    return domain::Application(domain::ApplicationId(id), domain::ApplicationName(name),
                               domain::LaunchTarget::process({"/bin/" + id}));
}

}  // namespace

TEST(FileApplicationRepositoryTest, LoadsAndSortsManifests) {
    TempDirectory directory;
    directory.writeFile("zeta.app", "name=Zeta\nexec=/bin/zeta\n");
    directory.writeFile("alpha.app", "name=alpha\nexec=/bin/alpha\n");

    infrastructure::FileApplicationRepository repository({directory.path()});
    const auto applications = repository.findAll();

    ASSERT_EQ(applications.size(), 2u);
    EXPECT_EQ(applications[0].name().value(), "alpha");  // Case-insensitive order.
    EXPECT_EQ(applications[1].name().value(), "Zeta");
}

TEST(FileApplicationRepositoryTest, SkipsInvalidManifestsAndForeignFiles) {
    TempDirectory directory;
    directory.writeFile("valid.app", "name=Valid\nexec=/bin/valid\n");
    directory.writeFile("broken.app", "exec=/bin/broken\n");  // Missing name.
    directory.writeFile("notes.txt", "name=NotAManifest\nexec=/bin/x\n");

    infrastructure::FileApplicationRepository repository({directory.path()});
    const auto applications = repository.findAll();

    ASSERT_EQ(applications.size(), 1u);
    EXPECT_EQ(applications[0].id().value(), "valid");
}

TEST(FileApplicationRepositoryTest, IgnoresMissingDirectories) {
    infrastructure::FileApplicationRepository repository({"/nonexistent/reboard-test"});
    EXPECT_TRUE(repository.findAll().empty());
}

TEST(FileApplicationRepositoryTest, MergesMultipleDirectories) {
    TempDirectory first;
    TempDirectory second;
    first.writeFile("a.app", "name=A\nexec=/bin/a\n");
    second.writeFile("b.app", "name=B\nexec=/bin/b\n");

    infrastructure::FileApplicationRepository repository({first.path(), second.path()});
    EXPECT_EQ(repository.findAll().size(), 2u);
}

TEST(CompositeApplicationRepositoryTest, FirstSourceWinsOnDuplicateIds) {
    StaticApplicationRepository userApps({makeApp("xochitl", "My Custom Notebooks")});
    infrastructure::BuiltInApplicationRepository builtIns;
    infrastructure::CompositeApplicationRepository composite({&userApps, &builtIns});

    const auto applications = composite.findAll();
    ASSERT_EQ(applications.size(), 1u);
    EXPECT_EQ(applications[0].name().value(), "My Custom Notebooks");
}

TEST(CompositeApplicationRepositoryTest, ConcatenatesDistinctApplications) {
    StaticApplicationRepository userApps({makeApp("koreader", "KOReader")});
    infrastructure::BuiltInApplicationRepository builtIns;
    infrastructure::CompositeApplicationRepository composite({&userApps, &builtIns});

    EXPECT_EQ(composite.findAll().size(), 2u);
}

TEST(BuiltInApplicationRepositoryTest, ProvidesTheXochitlEntry) {
    infrastructure::BuiltInApplicationRepository repository;
    const auto applications = repository.findAll();

    ASSERT_EQ(applications.size(), 1u);
    EXPECT_EQ(applications[0].id().value(), "xochitl");
    EXPECT_EQ(applications[0].launchTarget().type(), domain::LaunchType::SystemdUnit);
    EXPECT_TRUE(applications[0].pinnedToDock());
}

TEST(InMemorySessionRepositoryTest, TracksForegroundLifecycle) {
    infrastructure::InMemorySessionRepository session;
    EXPECT_FALSE(session.foreground());

    session.setForeground(domain::ForegroundApplication(domain::ApplicationId("app"),
                                                        domain::ProcessHandle::forPid(1)));
    ASSERT_TRUE(session.foreground());
    EXPECT_EQ(session.foreground()->id().value(), "app");

    session.clear();
    EXPECT_FALSE(session.foreground());
}
