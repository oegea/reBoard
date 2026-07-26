#include <gtest/gtest.h>

#include "domain/services/GestureDetector.h"

using namespace reboard::domain;

namespace {

TouchSample down(double x, double y, std::uint64_t t) { return {x, y, t, TouchPhase::Down}; }
TouchSample move(double x, double y, std::uint64_t t) { return {x, y, t, TouchPhase::Move}; }
TouchSample up(double x, double y, std::uint64_t t) { return {x, y, t, TouchPhase::Up}; }

}  // namespace

TEST(GestureDetectorTest, DetectsSwipeUpFromBottomWhileMoving) {
    GestureDetector detector;
    EXPECT_FALSE(detector.feed(down(0.5, 0.98, 0)));
    EXPECT_FALSE(detector.feed(move(0.5, 0.90, 100)));
    const auto gesture = detector.feed(move(0.5, 0.70, 200));
    ASSERT_TRUE(gesture);
    EXPECT_EQ(gesture->type(), GestureType::SwipeUpFromBottom);
}

TEST(GestureDetectorTest, DetectsSwipeUpOnRelease) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.95, 0));
    const auto gesture = detector.feed(up(0.5, 0.70, 300));
    ASSERT_TRUE(gesture);
    EXPECT_EQ(gesture->type(), GestureType::SwipeUpFromBottom);
}

TEST(GestureDetectorTest, IgnoresSwipesNotStartingAtTheBottomEdge) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.50, 0));
    EXPECT_FALSE(detector.feed(move(0.5, 0.20, 200)));
    EXPECT_FALSE(detector.feed(up(0.5, 0.10, 300)));
}

TEST(GestureDetectorTest, IgnoresSlowSwipes) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.98, 0));
    EXPECT_FALSE(detector.feed(up(0.5, 0.60, 5000)));
}

TEST(GestureDetectorTest, FiresSwipeOnlyOncePerTouch) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.98, 0));
    EXPECT_TRUE(detector.feed(move(0.5, 0.70, 100)));
    EXPECT_FALSE(detector.feed(move(0.5, 0.50, 200)));
    EXPECT_FALSE(detector.feed(up(0.5, 0.40, 300)));
}

TEST(GestureDetectorTest, DetectsLongPressThroughPolling) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.5, 1000));
    EXPECT_FALSE(detector.poll(2000));
    const auto gesture = detector.poll(2600);
    ASSERT_TRUE(gesture);
    EXPECT_EQ(gesture->type(), GestureType::LongPress);
    // Consumed: it must not fire again while the finger stays down.
    EXPECT_FALSE(detector.poll(5000));
}

TEST(GestureDetectorTest, MovementCancelsLongPress) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.5, 0));
    detector.feed(move(0.6, 0.6, 100));
    EXPECT_FALSE(detector.poll(3000));
}

TEST(GestureDetectorTest, ReleaseCancelsLongPress) {
    GestureDetector detector;
    detector.feed(down(0.5, 0.5, 0));
    detector.feed(up(0.5, 0.5, 100));
    EXPECT_FALSE(detector.poll(3000));
}

TEST(GestureDetectorTest, PollWithoutActiveTouchDoesNothing) {
    GestureDetector detector;
    EXPECT_FALSE(detector.poll(10000));
}

TEST(GestureDetectorTest, HonorsCustomConfiguration) {
    GestureDetectorConfig config;
    config.longPressDurationMs = 100;
    GestureDetector detector(config);
    detector.feed(down(0.5, 0.5, 0));
    EXPECT_TRUE(detector.poll(150));
}
