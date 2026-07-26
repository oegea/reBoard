#pragma once

#include <cstdint>
#include <optional>

#include "domain/valueobjects/Gesture.h"
#include "domain/valueobjects/TouchSample.h"

namespace reboard::domain {

struct GestureDetectorConfig {
    // A swipe must start below this normalized Y coordinate (screen bottom).
    double bottomEdgeStartY = 0.92;
    // Minimum upward travel (normalized) for a swipe-up gesture.
    double minSwipeDistanceY = 0.20;
    // Maximum duration of a swipe-up gesture.
    std::uint64_t maxSwipeDurationMs = 700;
    // How long a finger must stay pressed to trigger a long press.
    std::uint64_t longPressDurationMs = 1500;
    // Maximum movement (normalized, euclidean) tolerated during a long press.
    double longPressMaxMovement = 0.03;
};

// Pure, stateful recognizer for the "go home" gestures: swipe up from the
// bottom edge and long press. It knows nothing about input devices; it only
// consumes normalized TouchSamples and the current time.
class GestureDetector {
public:
    explicit GestureDetector(GestureDetectorConfig config = {});

    // Feed a touch sample. Returns a gesture at the moment it is recognized.
    std::optional<Gesture> feed(const TouchSample& sample);

    // Time-based check so long presses fire even without new input events.
    std::optional<Gesture> poll(std::uint64_t nowMs);

private:
    std::optional<Gesture> checkSwipeUp(double y, std::uint64_t timestampMs);
    std::optional<Gesture> checkLongPress(std::uint64_t nowMs);
    void trackMovement(double x, double y);

    GestureDetectorConfig config_;
    bool touchActive_ = false;
    bool gestureConsumed_ = false;
    bool startedAtBottomEdge_ = false;
    double startX_ = 0.0;
    double startY_ = 0.0;
    double maxMovement_ = 0.0;
    std::uint64_t startTimestampMs_ = 0;
};

}  // namespace reboard::domain
