#pragma once

#include <stdexcept>
#include <string>

namespace reboard::domain {

// Immutable human-readable name of an application, as shown on the board.
class ApplicationName {
public:
    explicit ApplicationName(std::string value)
        : value_(trim(std::move(value))) {
        if (value_.empty()) {
            throw std::invalid_argument("ApplicationName cannot be empty");
        }
    }

    const std::string& value() const noexcept { return value_; }

    bool operator==(const ApplicationName& other) const noexcept { return value_ == other.value_; }
    bool operator!=(const ApplicationName& other) const noexcept { return !(*this == other); }

private:
    static std::string trim(std::string value) {
        const auto begin = value.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) {
            return "";
        }
        const auto end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    std::string value_;
};

}  // namespace reboard::domain
