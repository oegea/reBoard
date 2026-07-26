#pragma once

#include <stdexcept>
#include <string>

namespace reboard::domain {

enum class ProcessHandleType {
    Pid,          // A process launched directly by reBoard.
    SystemdUnit,  // A systemd service managed through systemctl.
};

// Immutable reference to a running (or previously running) application so it
// can be monitored and stopped later.
class ProcessHandle {
public:
    static ProcessHandle forPid(int pid) {
        if (pid <= 0) {
            throw std::invalid_argument("Process id must be positive");
        }
        return ProcessHandle(ProcessHandleType::Pid, pid, "");
    }

    static ProcessHandle forUnit(std::string unitName) {
        if (unitName.empty()) {
            throw std::invalid_argument("Systemd unit name cannot be empty");
        }
        return ProcessHandle(ProcessHandleType::SystemdUnit, -1, std::move(unitName));
    }

    ProcessHandleType type() const noexcept { return type_; }

    int pid() const {
        if (type_ != ProcessHandleType::Pid) {
            throw std::logic_error("pid() is only available for pid handles");
        }
        return pid_;
    }

    const std::string& unitName() const {
        if (type_ != ProcessHandleType::SystemdUnit) {
            throw std::logic_error("unitName() is only available for systemd unit handles");
        }
        return unitName_;
    }

    bool operator==(const ProcessHandle& other) const noexcept {
        return type_ == other.type_ && pid_ == other.pid_ && unitName_ == other.unitName_;
    }
    bool operator!=(const ProcessHandle& other) const noexcept { return !(*this == other); }

private:
    ProcessHandle(ProcessHandleType type, int pid, std::string unitName)
        : type_(type), pid_(pid), unitName_(std::move(unitName)) {}

    ProcessHandleType type_;
    int pid_;
    std::string unitName_;
};

}  // namespace reboard::domain
