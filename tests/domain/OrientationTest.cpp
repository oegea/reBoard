#include <gtest/gtest.h>

#include "domain/services/OrientationPolicy.h"
#include "domain/services/TouchCoordinateTransform.h"

using namespace reboard::domain;

TEST(OrientationPolicyTest, PortraitByDefault) {
    EXPECT_EQ(OrientationPolicy::decide(false), Orientation::Portrait);
}

TEST(OrientationPolicyTest, LandscapeWhenKeyboardPresent) {
    EXPECT_EQ(OrientationPolicy::decide(true), Orientation::Landscape);
}

TEST(OrientationPolicyTest, ManualOverrideWins) {
    EXPECT_EQ(OrientationPolicy::decide(true, std::string("portrait")), Orientation::Portrait);
    EXPECT_EQ(OrientationPolicy::decide(false, std::string("landscape")), Orientation::Landscape);
}

TEST(OrientationPolicyTest, UnknownOverrideIsIgnored) {
    EXPECT_EQ(OrientationPolicy::decide(true, std::string("diagonal")), Orientation::Landscape);
    EXPECT_EQ(OrientationPolicy::decide(false, std::string("")), Orientation::Portrait);
}

TEST(OrientationPolicyTest, RotationDegrees) {
    EXPECT_EQ(OrientationPolicy::rotationDegrees(Orientation::Portrait), 0);
    EXPECT_EQ(OrientationPolicy::rotationDegrees(Orientation::Landscape), 90);
    EXPECT_EQ(OrientationPolicy::rotationDegrees(Orientation::Landscape, 270), 270);
    // Invalid landscape rotations fall back to the default.
    EXPECT_EQ(OrientationPolicy::rotationDegrees(Orientation::Landscape, 180), 90);
}

TEST(TouchCoordinateTransformTest, IdentityInPortrait) {
    const TouchSample sample(0.25, 0.75, 10, TouchPhase::Move);
    const auto rotated = rotateTouchSample(sample, 0);
    EXPECT_DOUBLE_EQ(rotated.x(), 0.25);
    EXPECT_DOUBLE_EQ(rotated.y(), 0.75);
}

TEST(TouchCoordinateTransformTest, PhysicalLeftEdgeIsVisualBottomAt90) {
    // Content rotated 90° clockwise: the visual bottom lies on the physical
    // left edge (x = 0).
    const TouchSample sample(0.0, 0.5, 10, TouchPhase::Down);
    const auto rotated = rotateTouchSample(sample, 90);
    EXPECT_DOUBLE_EQ(rotated.y(), 1.0);
    EXPECT_DOUBLE_EQ(rotated.x(), 0.5);
}

TEST(TouchCoordinateTransformTest, PhysicalRightEdgeIsVisualBottomAt270) {
    const TouchSample sample(1.0, 0.5, 10, TouchPhase::Down);
    const auto rotated = rotateTouchSample(sample, 270);
    EXPECT_DOUBLE_EQ(rotated.y(), 1.0);
    EXPECT_DOUBLE_EQ(rotated.x(), 0.5);
}

TEST(TouchCoordinateTransformTest, PreservesTimestampAndPhase) {
    const TouchSample sample(0.3, 0.9, 42, TouchPhase::Up);
    const auto rotated = rotateTouchSample(sample, 90);
    EXPECT_EQ(rotated.timestampMs(), 42u);
    EXPECT_EQ(rotated.phase(), TouchPhase::Up);
}
