#include <gtest/gtest.h>

#include "infrastructure/processes/UiDirective.h"

using reboard::infrastructure::parseLaunchDirective;

TEST(UiDirectiveTest, ParsesLaunchDirective) {
    const auto id = parseLaunchDirective("launch:koreader\n");
    ASSERT_TRUE(id);
    EXPECT_EQ(*id, "koreader");
}

TEST(UiDirectiveTest, LastDirectiveWins) {
    const auto id = parseLaunchDirective("launch:first\nlaunch:second\n");
    ASSERT_TRUE(id);
    EXPECT_EQ(*id, "second");
}

TEST(UiDirectiveTest, IgnoresUnrelatedOutputAndWhitespace) {
    const auto id = parseLaunchDirective("qml: some debug noise\n  launch: xochitl \r\nbye\n");
    ASSERT_TRUE(id);
    EXPECT_EQ(*id, "xochitl");
}

TEST(UiDirectiveTest, ReturnsNothingWithoutDirective) {
    EXPECT_FALSE(parseLaunchDirective(""));
    EXPECT_FALSE(parseLaunchDirective("hello\nworld\n"));
    EXPECT_FALSE(parseLaunchDirective("launch:\n"));  // Empty id is not a directive.
}
