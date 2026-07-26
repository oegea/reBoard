#include <gtest/gtest.h>

#include "domain/valueobjects/ApplicationId.h"
#include "domain/valueobjects/ApplicationName.h"
#include "domain/valueobjects/Gesture.h"
#include "domain/valueobjects/LaunchTarget.h"
#include "domain/valueobjects/ProcessHandle.h"
#include "domain/valueobjects/TouchSample.h"

using namespace reboard::domain;

TEST(ApplicationIdTest, AcceptsSlugCharacters) {
    const ApplicationId id("my-app_1.2");
    EXPECT_EQ(id.value(), "my-app_1.2");
}

TEST(ApplicationIdTest, RejectsEmptyValue) {
    EXPECT_THROW(ApplicationId(""), std::invalid_argument);
}

TEST(ApplicationIdTest, RejectsInvalidCharacters) {
    EXPECT_THROW(ApplicationId("bad id"), std::invalid_argument);
    EXPECT_THROW(ApplicationId("bad/id"), std::invalid_argument);
    EXPECT_THROW(ApplicationId("bad;id"), std::invalid_argument);
}

TEST(ApplicationIdTest, ComparesByValue) {
    EXPECT_EQ(ApplicationId("a"), ApplicationId("a"));
    EXPECT_NE(ApplicationId("a"), ApplicationId("b"));
}

TEST(ApplicationNameTest, TrimsSurroundingWhitespace) {
    EXPECT_EQ(ApplicationName("  KOReader \t").value(), "KOReader");
}

TEST(ApplicationNameTest, RejectsEmptyOrBlankValues) {
    EXPECT_THROW(ApplicationName(""), std::invalid_argument);
    EXPECT_THROW(ApplicationName("   \t "), std::invalid_argument);
}

TEST(LaunchTargetTest, BuildsProcessTargets) {
    const auto target = LaunchTarget::process({"/usr/bin/app", "--flag"});
    EXPECT_EQ(target.type(), LaunchType::Process);
    EXPECT_EQ(target.argv().size(), 2u);
    EXPECT_THROW(target.unitName(), std::logic_error);
}

TEST(LaunchTargetTest, RejectsEmptyProcessCommands) {
    EXPECT_THROW(LaunchTarget::process({}), std::invalid_argument);
    EXPECT_THROW(LaunchTarget::process({""}), std::invalid_argument);
}

TEST(LaunchTargetTest, BuildsSystemdUnitTargets) {
    const auto target = LaunchTarget::systemdUnit("xochitl");
    EXPECT_EQ(target.type(), LaunchType::SystemdUnit);
    EXPECT_EQ(target.unitName(), "xochitl");
    EXPECT_THROW(target.argv(), std::logic_error);
}

TEST(LaunchTargetTest, RejectsInvalidUnitNames) {
    EXPECT_THROW(LaunchTarget::systemdUnit(""), std::invalid_argument);
    EXPECT_THROW(LaunchTarget::systemdUnit("unit name"), std::invalid_argument);
    EXPECT_THROW(LaunchTarget::systemdUnit("unit;rm"), std::invalid_argument);
}

TEST(LaunchTargetTest, ComparesByValue) {
    EXPECT_EQ(LaunchTarget::systemdUnit("a"), LaunchTarget::systemdUnit("a"));
    EXPECT_NE(LaunchTarget::systemdUnit("a"), LaunchTarget::process({"a"}));
}

TEST(ProcessHandleTest, BuildsPidHandles) {
    const auto handle = ProcessHandle::forPid(42);
    EXPECT_EQ(handle.type(), ProcessHandleType::Pid);
    EXPECT_EQ(handle.pid(), 42);
    EXPECT_THROW(handle.unitName(), std::logic_error);
}

TEST(ProcessHandleTest, RejectsNonPositivePids) {
    EXPECT_THROW(ProcessHandle::forPid(0), std::invalid_argument);
    EXPECT_THROW(ProcessHandle::forPid(-1), std::invalid_argument);
}

TEST(ProcessHandleTest, BuildsUnitHandles) {
    const auto handle = ProcessHandle::forUnit("xochitl");
    EXPECT_EQ(handle.type(), ProcessHandleType::SystemdUnit);
    EXPECT_EQ(handle.unitName(), "xochitl");
    EXPECT_THROW(handle.pid(), std::logic_error);
    EXPECT_THROW(ProcessHandle::forUnit(""), std::invalid_argument);
}

TEST(TouchSampleTest, StoresNormalizedCoordinates) {
    const TouchSample sample(0.5, 1.0, 123, TouchPhase::Down);
    EXPECT_DOUBLE_EQ(sample.x(), 0.5);
    EXPECT_DOUBLE_EQ(sample.y(), 1.0);
    EXPECT_EQ(sample.timestampMs(), 123u);
    EXPECT_EQ(sample.phase(), TouchPhase::Down);
}

TEST(TouchSampleTest, RejectsOutOfRangeCoordinates) {
    EXPECT_THROW(TouchSample(-0.1, 0.5, 0, TouchPhase::Down), std::invalid_argument);
    EXPECT_THROW(TouchSample(0.5, 1.1, 0, TouchPhase::Down), std::invalid_argument);
}

TEST(GestureTest, StoresTypeAndTimestamp) {
    const Gesture gesture(GestureType::LongPress, 99);
    EXPECT_EQ(gesture.type(), GestureType::LongPress);
    EXPECT_EQ(gesture.timestampMs(), 99u);
}
