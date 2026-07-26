#include "infrastructure/processes/PosixProcessRepository.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace reboard::infrastructure {

PosixProcessRepository::PosixProcessRepository(std::string systemctlPath)
    : systemctlPath_(std::move(systemctlPath)) {}

domain::ProcessHandle PosixProcessRepository::launch(const domain::LaunchTarget& target) {
    if (target.type() == domain::LaunchType::SystemdUnit) {
        // Quick open/close cycles can exhaust the unit's systemd start rate
        // limit; a refused start marks the unit as failed, and on reMarkable
        // xochitl's OnFailure= handler then REBOOTS the device. Clearing the
        // failure counter first makes relaunching always safe.
        runSystemctl("reset-failed", target.unitName());
        if (runSystemctl("start", target.unitName()) != 0) {
            // A false negative here is dangerous: concluding "not running"
            // while the unit is actually coming up makes the caller show the
            // launcher again and collide on the e-paper display. Trust
            // is-active before giving up.
            ::usleep(500 * 1000);
            if (runSystemctl("is-active", target.unitName()) != 0) {
                throw std::runtime_error("systemctl start failed for unit " + target.unitName());
            }
        }
        return domain::ProcessHandle::forUnit(target.unitName());
    }

    const auto& argv = target.argv();
    std::vector<char*> argvPointers;
    argvPointers.reserve(argv.size() + 1);
    for (const auto& argument : argv) {
        argvPointers.push_back(const_cast<char*>(argument.c_str()));
    }
    argvPointers.push_back(nullptr);

    const pid_t pid = ::fork();
    if (pid < 0) {
        throw std::runtime_error(std::string("fork failed: ") + std::strerror(errno));
    }
    if (pid == 0) {
        // Child: detach into its own session so the whole application tree
        // can be stopped later by signalling the process group.
        ::setsid();
        ::execvp(argvPointers[0], argvPointers.data());
        ::_exit(127);
    }
    return domain::ProcessHandle::forPid(static_cast<int>(pid));
}

void PosixProcessRepository::stop(const domain::ProcessHandle& handle) {
    if (handle.type() == domain::ProcessHandleType::SystemdUnit) {
        runSystemctl("stop", handle.unitName());
        return;
    }
    // Negative pid signals the whole process group created by setsid(). If
    // the child has not called setsid() yet (launch/stop race), the group
    // does not exist, so fall back to signalling the process directly.
    if (::kill(-handle.pid(), SIGTERM) != 0) {
        ::kill(handle.pid(), SIGTERM);
    }
    if (waitForExit(handle.pid(), 2000)) {
        return;
    }
    // The application ignored SIGTERM; the display must be freed, so kill it.
    if (::kill(-handle.pid(), SIGKILL) != 0) {
        ::kill(handle.pid(), SIGKILL);
    }
    waitForExit(handle.pid(), 1000);
}

bool PosixProcessRepository::waitForExit(int pid, int timeoutMs) {
    const int pollIntervalMs = 20;
    for (int elapsed = 0; elapsed <= timeoutMs; elapsed += pollIntervalMs) {
        const pid_t waited = ::waitpid(pid, nullptr, WNOHANG);
        if (waited == pid) {
            return true;  // Reaped: it is gone.
        }
        if (waited < 0 && ::kill(pid, 0) != 0) {
            return true;  // Not our child and no longer alive.
        }
        ::usleep(pollIntervalMs * 1000);
    }
    return false;
}

bool PosixProcessRepository::isRunning(const domain::ProcessHandle& handle) const {
    if (handle.type() == domain::ProcessHandleType::SystemdUnit) {
        return runSystemctl("is-active", handle.unitName()) == 0;
    }

    // Reap the child if it already exited; this keeps the resident launcher
    // free of zombies because the UI refreshes the foreground state
    // periodically.
    int status = 0;
    const pid_t waited = ::waitpid(handle.pid(), &status, WNOHANG);
    if (waited == handle.pid()) {
        return false;  // Just reaped: it is no longer running.
    }
    if (waited == 0) {
        return true;  // Still running.
    }
    // Not our child (or already reaped): fall back to a liveness probe.
    return ::kill(handle.pid(), 0) == 0;
}

int PosixProcessRepository::runSystemctl(const std::string& verb,
                                         const std::string& unitName) const {
    const pid_t pid = ::fork();
    if (pid < 0) {
        throw std::runtime_error(std::string("fork failed: ") + std::strerror(errno));
    }
    if (pid == 0) {
        const char* argv[] = {systemctlPath_.c_str(), "--quiet", verb.c_str(), unitName.c_str(),
                              nullptr};
        ::execvp(systemctlPath_.c_str(), const_cast<char* const*>(argv));
        ::_exit(127);
    }

    // The daemon handles SIGTERM/SIGINT without SA_RESTART, so this wait can
    // be interrupted; treating EINTR as a failure would report a bogus error
    // for a systemctl call that actually succeeded.
    int status = 0;
    pid_t waited;
    do {
        waited = ::waitpid(pid, &status, 0);
    } while (waited < 0 && errno == EINTR);
    if (waited != pid) {
        return -1;
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

}  // namespace reboard::infrastructure
