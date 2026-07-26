#pragma once

#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace reboard::domain {

enum class LaunchType {
    Process,      // A third-party binary executed directly.
    SystemdUnit,  // A systemd service started/stopped through systemctl.
};

// Immutable description of how an application is started. reBoard never embeds
// applications: every target points at an external binary or systemd unit.
class LaunchTarget {
public:
    static LaunchTarget process(std::vector<std::string> argv) {
        if (argv.empty() || argv.front().empty()) {
            throw std::invalid_argument("Process launch target requires a non-empty command");
        }
        return LaunchTarget(LaunchType::Process, std::move(argv), "");
    }

    static LaunchTarget systemdUnit(std::string unitName) {
        if (unitName.empty()) {
            throw std::invalid_argument("Systemd unit name cannot be empty");
        }
        for (const char c : unitName) {
            if (!isAllowedUnitChar(c)) {
                throw std::invalid_argument("Systemd unit name contains invalid character: " + unitName);
            }
        }
        return LaunchTarget(LaunchType::SystemdUnit, {}, std::move(unitName));
    }

    LaunchType type() const noexcept { return type_; }

    const std::vector<std::string>& argv() const {
        if (type_ != LaunchType::Process) {
            throw std::logic_error("argv() is only available for process launch targets");
        }
        return argv_;
    }

    const std::string& unitName() const {
        if (type_ != LaunchType::SystemdUnit) {
            throw std::logic_error("unitName() is only available for systemd unit launch targets");
        }
        return unitName_;
    }

    bool operator==(const LaunchTarget& other) const noexcept {
        return type_ == other.type_ && argv_ == other.argv_ && unitName_ == other.unitName_;
    }
    bool operator!=(const LaunchTarget& other) const noexcept { return !(*this == other); }

private:
    LaunchTarget(LaunchType type, std::vector<std::string> argv, std::string unitName)
        : type_(type), argv_(std::move(argv)), unitName_(std::move(unitName)) {}

    static bool isAllowedUnitChar(char c) noexcept {
        const auto uc = static_cast<unsigned char>(c);
        return std::isalnum(uc) != 0 || c == '-' || c == '_' || c == '.' || c == '@' || c == '\\' || c == ':';
    }

    LaunchType type_;
    std::vector<std::string> argv_;
    std::string unitName_;
};

}  // namespace reboard::domain
