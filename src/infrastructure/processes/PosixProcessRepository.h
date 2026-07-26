#pragma once

#include <string>

#include "domain/repositories/ProcessRepository.h"

namespace reboard::infrastructure {

// Launches and controls external applications using POSIX primitives:
// - Process targets are fork/exec'd in their own session (process group).
// - Systemd unit targets are controlled through the systemctl binary, which
//   is how the official reMarkable OS manages xochitl.
class PosixProcessRepository : public domain::ProcessRepository {
public:
    // `systemctlPath` is injectable so tests never touch the real systemd
    // (starting units as a regular user triggers polkit password prompts).
    explicit PosixProcessRepository(std::string systemctlPath = "systemctl");

    domain::ProcessHandle launch(const domain::LaunchTarget& target) override;

    // Blocks until the target is dead: SIGTERM, then SIGKILL after a grace
    // period. `systemctl stop` is synchronous by itself.
    void stop(const domain::ProcessHandle& handle) override;

    bool isRunning(const domain::ProcessHandle& handle) const override;

private:
    int runSystemctl(const std::string& verb, const std::string& unitName) const;
    static bool waitForExit(int pid, int timeoutMs);

    std::string systemctlPath_;
};

}  // namespace reboard::infrastructure
