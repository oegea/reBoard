#include <gtest/gtest.h>

#include <optional>
#include <stdexcept>
#include <vector>

#include "application/usecases/AdoptRunningApplicationUseCase.h"
#include "application/usecases/CloseForegroundApplicationUseCase.h"
#include "application/usecases/GetBoardUseCase.h"
#include "application/usecases/LaunchApplicationUseCase.h"
#include "application/usecases/RefreshForegroundStateUseCase.h"

using namespace reboard;

namespace {

class FakeApplicationRepository : public domain::ApplicationRepository {
public:
    std::vector<domain::Application> applications;

    std::vector<domain::Application> findAll() const override { return applications; }
};

class FakeProcessRepository : public domain::ProcessRepository {
public:
    mutable std::vector<domain::LaunchTarget> launched;
    std::vector<domain::ProcessHandle> stopped;
    std::vector<domain::ProcessHandle> runningHandles;
    bool failNextLaunch = false;
    int nextPid = 100;

    domain::ProcessHandle launch(const domain::LaunchTarget& target) override {
        if (failNextLaunch) {
            throw std::runtime_error("launch failed");
        }
        launched.push_back(target);
        if (target.type() == domain::LaunchType::SystemdUnit) {
            return domain::ProcessHandle::forUnit(target.unitName());
        }
        return domain::ProcessHandle::forPid(nextPid++);
    }

    void stop(const domain::ProcessHandle& handle) override { stopped.push_back(handle); }

    bool isRunning(const domain::ProcessHandle& handle) const override {
        for (const auto& running : runningHandles) {
            if (running == handle) {
                return true;
            }
        }
        return false;
    }

    bool lastExitWasAbnormal(const domain::ProcessHandle& handle) const override {
        for (const auto& crashed : abnormalHandles) {
            if (crashed == handle) {
                return true;
            }
        }
        return false;
    }

    std::vector<domain::ProcessHandle> abnormalHandles;
};

class FakeSessionRepository : public domain::SessionRepository {
public:
    std::optional<domain::ForegroundApplication> current;

    void setForeground(const domain::ForegroundApplication& foreground) override {
        current = foreground;
    }
    std::optional<domain::ForegroundApplication> foreground() const override { return current; }
    void clear() override { current.reset(); }
};

domain::Application processApp(const std::string& id) {
    return domain::Application(domain::ApplicationId(id), domain::ApplicationName("App " + id),
                               domain::LaunchTarget::process({"/bin/" + id}));
}

domain::Application unitApp(const std::string& id, const std::string& unit, bool dock = false) {
    return domain::Application(domain::ApplicationId(id), domain::ApplicationName("App " + id),
                               domain::LaunchTarget::systemdUnit(unit), "", dock);
}

}  // namespace

TEST(GetBoardUseCaseTest, OrganizesApplicationsIntoBoard) {
    FakeApplicationRepository applications;
    applications.applications = {processApp("a"), unitApp("xochitl", "xochitl", true)};

    application::GetBoardUseCase useCase(applications);
    const auto board = useCase.execute();

    ASSERT_EQ(board.dock().size(), 1u);
    EXPECT_EQ(board.dock()[0].id().value(), "xochitl");
    ASSERT_EQ(board.pages().size(), 1u);
    EXPECT_EQ(board.pages()[0][0].id().value(), "a");
}

TEST(LaunchApplicationUseCaseTest, ReturnsNotFoundForUnknownId) {
    FakeApplicationRepository applications;
    FakeProcessRepository processes;
    FakeSessionRepository session;
    application::LaunchApplicationUseCase useCase(applications, processes, session);

    EXPECT_EQ(useCase.execute(domain::ApplicationId("missing")),
              application::LaunchResult::NotFound);
    EXPECT_TRUE(processes.launched.empty());
}

TEST(LaunchApplicationUseCaseTest, LaunchesAndTracksForeground) {
    FakeApplicationRepository applications;
    applications.applications = {processApp("koreader")};
    FakeProcessRepository processes;
    FakeSessionRepository session;
    application::LaunchApplicationUseCase useCase(applications, processes, session);

    EXPECT_EQ(useCase.execute(domain::ApplicationId("koreader")),
              application::LaunchResult::Launched);
    ASSERT_EQ(processes.launched.size(), 1u);
    ASSERT_TRUE(session.current);
    EXPECT_EQ(session.current->id().value(), "koreader");
    EXPECT_EQ(session.current->handle().type(), domain::ProcessHandleType::Pid);
}

TEST(LaunchApplicationUseCaseTest, StopsPreviousForegroundBeforeLaunching) {
    FakeApplicationRepository applications;
    applications.applications = {processApp("koreader")};
    FakeProcessRepository processes;
    FakeSessionRepository session;
    const auto previousHandle = domain::ProcessHandle::forUnit("xochitl");
    session.current = domain::ForegroundApplication(domain::ApplicationId("xochitl"),
                                                    previousHandle);
    application::LaunchApplicationUseCase useCase(applications, processes, session);

    EXPECT_EQ(useCase.execute(domain::ApplicationId("koreader")),
              application::LaunchResult::Launched);
    ASSERT_EQ(processes.stopped.size(), 1u);
    EXPECT_EQ(processes.stopped[0], previousHandle);
    EXPECT_EQ(session.current->id().value(), "koreader");
}

TEST(CloseForegroundApplicationUseCaseTest, ReturnsFalseWhenNothingIsForeground) {
    FakeProcessRepository processes;
    FakeSessionRepository session;
    application::CloseForegroundApplicationUseCase useCase(processes, session);

    EXPECT_FALSE(useCase.execute());
    EXPECT_TRUE(processes.stopped.empty());
}

TEST(CloseForegroundApplicationUseCaseTest, StopsAndClearsForeground) {
    FakeProcessRepository processes;
    FakeSessionRepository session;
    const auto handle = domain::ProcessHandle::forPid(55);
    session.current = domain::ForegroundApplication(domain::ApplicationId("app"), handle);
    application::CloseForegroundApplicationUseCase useCase(processes, session);

    EXPECT_TRUE(useCase.execute());
    ASSERT_EQ(processes.stopped.size(), 1u);
    EXPECT_EQ(processes.stopped[0], handle);
    EXPECT_FALSE(session.current);
}

TEST(AdoptRunningApplicationUseCaseTest, AdoptsRunningUnitApplication) {
    FakeApplicationRepository applications;
    applications.applications = {processApp("koreader"), unitApp("xochitl", "xochitl", true)};
    FakeProcessRepository processes;
    processes.runningHandles = {domain::ProcessHandle::forUnit("xochitl")};
    FakeSessionRepository session;
    application::AdoptRunningApplicationUseCase useCase(applications, processes, session);

    EXPECT_TRUE(useCase.execute());
    ASSERT_TRUE(session.current);
    EXPECT_EQ(session.current->id().value(), "xochitl");
}

TEST(AdoptRunningApplicationUseCaseTest, DoesNothingWhenNothingRuns) {
    FakeApplicationRepository applications;
    applications.applications = {unitApp("xochitl", "xochitl", true)};
    FakeProcessRepository processes;
    FakeSessionRepository session;
    application::AdoptRunningApplicationUseCase useCase(applications, processes, session);

    EXPECT_FALSE(useCase.execute());
    EXPECT_FALSE(session.current);
}

TEST(AdoptRunningApplicationUseCaseTest, KeepsExistingForeground) {
    FakeApplicationRepository applications;
    applications.applications = {unitApp("xochitl", "xochitl", true)};
    FakeProcessRepository processes;
    processes.runningHandles = {domain::ProcessHandle::forUnit("xochitl")};
    FakeSessionRepository session;
    session.current = domain::ForegroundApplication(domain::ApplicationId("other"),
                                                    domain::ProcessHandle::forPid(9));
    application::AdoptRunningApplicationUseCase useCase(applications, processes, session);

    EXPECT_FALSE(useCase.execute());
    EXPECT_EQ(session.current->id().value(), "other");
}

TEST(RefreshForegroundStateUseCaseTest, ReportsNoneWithoutForeground) {
    FakeProcessRepository processes;
    FakeSessionRepository session;
    application::RefreshForegroundStateUseCase useCase(processes, session);

    EXPECT_EQ(useCase.execute(), application::ForegroundState::None);
}

TEST(RefreshForegroundStateUseCaseTest, ReportsRunningForeground) {
    FakeProcessRepository processes;
    const auto handle = domain::ProcessHandle::forPid(70);
    processes.runningHandles = {handle};
    FakeSessionRepository session;
    session.current = domain::ForegroundApplication(domain::ApplicationId("app"), handle);
    application::RefreshForegroundStateUseCase useCase(processes, session);

    EXPECT_EQ(useCase.execute(), application::ForegroundState::Running);
    EXPECT_TRUE(session.current);
}

TEST(RefreshForegroundStateUseCaseTest, ReportsCrashWhenExitWasAbnormal) {
    FakeProcessRepository processes;
    const auto handle = domain::ProcessHandle::forPid(71);
    processes.abnormalHandles = {handle};
    FakeSessionRepository session;
    session.current = domain::ForegroundApplication(domain::ApplicationId("app"), handle);
    application::RefreshForegroundStateUseCase useCase(processes, session);

    EXPECT_EQ(useCase.execute(), application::ForegroundState::Crashed);
    EXPECT_FALSE(session.current);
}

TEST(RefreshForegroundStateUseCaseTest, ClearsSessionWhenForegroundExited) {
    FakeProcessRepository processes;
    FakeSessionRepository session;
    session.current = domain::ForegroundApplication(domain::ApplicationId("app"),
                                                    domain::ProcessHandle::forPid(70));
    application::RefreshForegroundStateUseCase useCase(processes, session);

    EXPECT_EQ(useCase.execute(), application::ForegroundState::Exited);
    EXPECT_FALSE(session.current);
}
