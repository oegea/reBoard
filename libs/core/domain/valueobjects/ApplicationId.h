#pragma once

#include <cctype>
#include <stdexcept>
#include <string>

namespace reboard::domain {

// Immutable identifier of an application. Restricted to a safe slug alphabet
// because ids come from manifest file names and are used in logs and lookups.
class ApplicationId {
public:
    explicit ApplicationId(std::string value)
        : value_(std::move(value)) {
        if (value_.empty()) {
            throw std::invalid_argument("ApplicationId cannot be empty");
        }
        for (const char c : value_) {
            if (!isAllowedChar(c)) {
                throw std::invalid_argument("ApplicationId contains invalid character: " + value_);
            }
        }
    }

    const std::string& value() const noexcept { return value_; }

    bool operator==(const ApplicationId& other) const noexcept { return value_ == other.value_; }
    bool operator!=(const ApplicationId& other) const noexcept { return !(*this == other); }

private:
    static bool isAllowedChar(char c) noexcept {
        const auto uc = static_cast<unsigned char>(c);
        return std::isalnum(uc) != 0 || c == '-' || c == '_' || c == '.';
    }

    std::string value_;
};

}  // namespace reboard::domain
