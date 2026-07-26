#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

#include "domain/valueobjects/TouchSample.h"

namespace reboard::infrastructure {

// Reads the touch screen through the Linux evdev interface (multitouch
// protocol B, first slot only) and converts raw events into normalized
// domain TouchSamples. Coordinates can be inverted per axis because e-paper
// devices mount the digitizer rotated (on reMarkable 2 the Y axis is
// inverted with respect to the screen).
class EvdevTouchScreen {
public:
    using SampleCallback = std::function<void(const domain::TouchSample&)>;
    using TickCallback = std::function<void(std::uint64_t nowMs)>;

    EvdevTouchScreen(std::string devicePath, bool invertX, bool invertY);

    // Scans /dev/input/event* for the first device reporting multitouch
    // absolute positions.
    static std::optional<std::string> findTouchDevice();

    // Blocking loop: emits samples as they arrive and ticks roughly every
    // 100 ms so time-based gestures (long press) can fire without new input.
    // Returns when `stopRequested` becomes true or on unrecoverable errors.
    void run(const SampleCallback& onSample, const TickCallback& onTick,
             const std::atomic<bool>& stopRequested);

private:
    std::string devicePath_;
    bool invertX_;
    bool invertY_;
};

}  // namespace reboard::infrastructure
