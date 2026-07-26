#pragma once

#include <cstdint>
#include <stdexcept>

namespace reboard::domain {

enum class TouchPhase {
    Down,
    Move,
    Up,
};

// Immutable, device-independent touch sample. Coordinates are normalized to
// [0, 1] with the origin at the top-left corner of the screen.
class TouchSample {
public:
    TouchSample(double x, double y, std::uint64_t timestampMs, TouchPhase phase)
        : x_(x), y_(y), timestampMs_(timestampMs), phase_(phase) {
        if (x_ < 0.0 || x_ > 1.0 || y_ < 0.0 || y_ > 1.0) {
            throw std::invalid_argument("TouchSample coordinates must be normalized to [0, 1]");
        }
    }

    double x() const noexcept { return x_; }
    double y() const noexcept { return y_; }
    std::uint64_t timestampMs() const noexcept { return timestampMs_; }
    TouchPhase phase() const noexcept { return phase_; }

private:
    double x_;
    double y_;
    std::uint64_t timestampMs_;
    TouchPhase phase_;
};

}  // namespace reboard::domain
