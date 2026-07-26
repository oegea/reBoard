#include "domain/services/GestureDetector.h"

#include <cmath>

namespace reboard::domain {

GestureDetector::GestureDetector(GestureDetectorConfig config)
    : config_(config) {}

std::optional<Gesture> GestureDetector::feed(const TouchSample& sample) {
    switch (sample.phase()) {
        case TouchPhase::Down:
            touchActive_ = true;
            gestureConsumed_ = false;
            startedAtBottomEdge_ = sample.y() >= config_.bottomEdgeStartY;
            startX_ = sample.x();
            startY_ = sample.y();
            maxMovement_ = 0.0;
            startTimestampMs_ = sample.timestampMs();
            return std::nullopt;

        case TouchPhase::Move: {
            if (!touchActive_ || gestureConsumed_) {
                return std::nullopt;
            }
            trackMovement(sample.x(), sample.y());
            if (auto swipe = checkSwipeUp(sample.y(), sample.timestampMs())) {
                return swipe;
            }
            return checkLongPress(sample.timestampMs());
        }

        case TouchPhase::Up: {
            if (!touchActive_ || gestureConsumed_) {
                touchActive_ = false;
                return std::nullopt;
            }
            trackMovement(sample.x(), sample.y());
            auto gesture = checkSwipeUp(sample.y(), sample.timestampMs());
            touchActive_ = false;
            return gesture;
        }
    }
    return std::nullopt;
}

std::optional<Gesture> GestureDetector::poll(std::uint64_t nowMs) {
    if (!touchActive_ || gestureConsumed_) {
        return std::nullopt;
    }
    return checkLongPress(nowMs);
}

std::optional<Gesture> GestureDetector::checkSwipeUp(double y, std::uint64_t timestampMs) {
    if (!startedAtBottomEdge_) {
        return std::nullopt;
    }
    const double travelledUp = startY_ - y;
    const std::uint64_t elapsed = timestampMs - startTimestampMs_;
    if (travelledUp >= config_.minSwipeDistanceY && elapsed <= config_.maxSwipeDurationMs) {
        gestureConsumed_ = true;
        return Gesture(GestureType::SwipeUpFromBottom, timestampMs);
    }
    return std::nullopt;
}

std::optional<Gesture> GestureDetector::checkLongPress(std::uint64_t nowMs) {
    // Both home gestures live at the bottom edge: a long press anywhere
    // else must never yank the user out of the running application.
    if (!startedAtBottomEdge_) {
        return std::nullopt;
    }
    if (maxMovement_ > config_.longPressMaxMovement) {
        return std::nullopt;
    }
    if (nowMs < startTimestampMs_ || nowMs - startTimestampMs_ < config_.longPressDurationMs) {
        return std::nullopt;
    }
    gestureConsumed_ = true;
    return Gesture(GestureType::LongPress, nowMs);
}

void GestureDetector::trackMovement(double x, double y) {
    const double dx = x - startX_;
    const double dy = y - startY_;
    const double distance = std::sqrt(dx * dx + dy * dy);
    if (distance > maxMovement_) {
        maxMovement_ = distance;
    }
}

}  // namespace reboard::domain
