#pragma once

#include <cstdint>

namespace reboard::domain {

enum class GestureType {
    SwipeUpFromBottom,
    LongPress,
};

// Immutable gesture recognized on the touch screen.
class Gesture {
public:
    Gesture(GestureType type, std::uint64_t timestampMs)
        : type_(type), timestampMs_(timestampMs) {}

    GestureType type() const noexcept { return type_; }
    std::uint64_t timestampMs() const noexcept { return timestampMs_; }

private:
    GestureType type_;
    std::uint64_t timestampMs_;
};

}  // namespace reboard::domain
