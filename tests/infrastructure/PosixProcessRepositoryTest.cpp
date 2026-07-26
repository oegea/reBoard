#include <gtest/gtest.h>

#include <cstdlib>
#include <chrono>
#include <thread>

#include "infrastructure/processes/PosixProcessRepository.h"

using namespace reboard;

namespace {

// Polls until the handle stops running or the timeout expires.
bool waitUntilStopped(const infrastructure::PosixProcessRepository& repository,
                      const domain::ProcessHandle& handle, int timeoutMs = 3000) {
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    while (std::chrono::steady_clock::now() < deadline) {
        if (!repository.isRunning(handle)) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return false;
}

}  // namespace

TEST(PosixProcessRepositoryTest, LaunchesAndStopsAProcess) {
    infrastructure::PosixProcessRepository repository;
    const auto handle = repository.launch(domain::LaunchTarget::process({"/bin/sleep", "30"}));

    ASSERT_EQ(handle.type(), domain::ProcessHandleType::Pid);
    EXPECT_TRUE(repository.isRunning(handle));

    repository.stop(handle);
    EXPECT_TRUE(waitUntilStopped(repository, handle));
}

TEST(PosixProcessRepositoryTest, DetectsWhenAProcessExitsOnItsOwn) {
    infrastructure::PosixProcessRepository repository;
    const auto handle = repository.launch(domain::LaunchTarget::process({"/bin/true"}));

    EXPECT_TRUE(waitUntilStopped(repository, handle));
}

TEST(PosixProcessRepositoryTest, FailedExecEndsUpNotRunning) {
    infrastructure::PosixProcessRepository repository;
    const auto handle = repository.launch(
        domain::LaunchTarget::process({"/nonexistent/reboard-test-binary"}));

    // The child exits with 127 right after the failed exec.
    EXPECT_TRUE(waitUntilStopped(repository, handle));
}

// Systemd interactions are exercised with fake systemctl binaries (/bin/true
// and /bin/false) so the tests never touch the real systemd — starting units
// as a regular user would trigger polkit password prompts on the host.

TEST(PosixProcessRepositoryTest, InactiveSystemdUnitIsNotRunning) {
    infrastructure::PosixProcessRepository repository("/bin/false");
    EXPECT_FALSE(
        repository.isRunning(domain::ProcessHandle::forUnit("reboard-test-nonexistent.service")));
}

TEST(PosixProcessRepositoryTest, FailingToStartASystemdUnitThrows) {
    infrastructure::PosixProcessRepository repository("/bin/false");
    EXPECT_THROW(
        repository.launch(domain::LaunchTarget::systemdUnit("reboard-test-nonexistent.service")),
        std::runtime_error);
}

TEST(PosixProcessRepositoryTest, ManagesSystemdUnitsThroughSystemctl) {
    infrastructure::PosixProcessRepository repository("/bin/true");
    const auto handle = repository.launch(domain::LaunchTarget::systemdUnit("xochitl"));

    ASSERT_EQ(handle.type(), domain::ProcessHandleType::SystemdUnit);
    EXPECT_EQ(handle.unitName(), "xochitl");
    EXPECT_TRUE(repository.isRunning(handle));
    EXPECT_NO_THROW(repository.stop(handle));
}

TEST(PosixProcessRepositoryTest, MissingSystemctlBinaryReportsFailure) {
    infrastructure::PosixProcessRepository repository("/nonexistent/reboard-systemctl");
    EXPECT_FALSE(repository.isRunning(domain::ProcessHandle::forUnit("xochitl")));
}
