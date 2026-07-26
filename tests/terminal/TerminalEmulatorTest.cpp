#include <gtest/gtest.h>

#include "emulator/TerminalEmulator.h"

using reboard::terminal::TerminalEmulator;

namespace {

std::string firstLine(const TerminalEmulator& emulator) {
    const std::string text = emulator.screenAsText();
    return text.substr(0, text.find('\n'));
}

}  // namespace

TEST(TerminalEmulatorTest, PrintsPlainText) {
    TerminalEmulator emulator(20, 4);
    emulator.feed("hello");
    EXPECT_EQ(firstLine(emulator), "hello");
    EXPECT_EQ(emulator.cursorColumn(), 5);
}

TEST(TerminalEmulatorTest, CarriageReturnAndLineFeed) {
    TerminalEmulator emulator(20, 4);
    emulator.feed("one\r\ntwo");
    EXPECT_EQ(emulator.screenAsText(), "one\ntwo\n\n\n");
    EXPECT_EQ(emulator.cursorRow(), 1);
}

TEST(TerminalEmulatorTest, DeferredAutowrap) {
    TerminalEmulator emulator(5, 3);
    emulator.feed("abcde");
    // The cursor sticks to the last column until the next glyph arrives.
    EXPECT_EQ(emulator.cursorRow(), 0);
    emulator.feed("f");
    EXPECT_EQ(emulator.cursorRow(), 1);
    EXPECT_EQ(emulator.screenAsText(), "abcde\nf\n\n");
}

TEST(TerminalEmulatorTest, CursorPositioning) {
    TerminalEmulator emulator(20, 5);
    emulator.feed("\033[3;5Hx");
    EXPECT_EQ(emulator.cursorRow(), 2);
    EXPECT_EQ(emulator.screen()[2][4].character, U'x');
}

TEST(TerminalEmulatorTest, EraseInLineAndDisplay) {
    TerminalEmulator emulator(10, 3);
    emulator.feed("abcdef\033[3D\033[K");
    EXPECT_EQ(firstLine(emulator), "abc");
    emulator.feed("\033[2J");
    EXPECT_EQ(emulator.screenAsText(), "\n\n\n");
}

TEST(TerminalEmulatorTest, SgrBoldAndInverse) {
    TerminalEmulator emulator(10, 2);
    emulator.feed("a\033[1mb\033[7mc\033[0md");
    EXPECT_FALSE(emulator.screen()[0][0].bold);
    EXPECT_TRUE(emulator.screen()[0][1].bold);
    EXPECT_TRUE(emulator.screen()[0][2].bold);
    EXPECT_TRUE(emulator.screen()[0][2].inverse);
    EXPECT_FALSE(emulator.screen()[0][3].bold);
}

TEST(TerminalEmulatorTest, AlternateScreenRestoresPrimary) {
    TerminalEmulator emulator(10, 3);
    emulator.feed("main");
    emulator.feed("\033[?1049h");
    EXPECT_TRUE(emulator.onAlternateScreen());
    emulator.feed("alt");
    EXPECT_EQ(firstLine(emulator), "alt");
    emulator.feed("\033[?1049l");
    EXPECT_FALSE(emulator.onAlternateScreen());
    EXPECT_EQ(firstLine(emulator), "main");
}

TEST(TerminalEmulatorTest, ScrollingFillsScrollback) {
    TerminalEmulator emulator(5, 2);
    emulator.feed("a\r\nb\r\nc");
    EXPECT_EQ(emulator.screenAsText(), "b\nc\n");
    ASSERT_EQ(emulator.scrollback().size(), 1u);
    EXPECT_EQ(emulator.scrollback()[0][0].character, U'a');
}

TEST(TerminalEmulatorTest, ScrollRegion) {
    TerminalEmulator emulator(5, 4);
    emulator.feed("1\r\n2\r\n3\r\n4");
    emulator.feed("\033[2;3r");   // Region: rows 2..3.
    emulator.feed("\033[3;1H\n");  // LF at region bottom scrolls the region.
    EXPECT_EQ(emulator.screenAsText(), "1\n3\n\n4\n");
}

TEST(TerminalEmulatorTest, ReportsCursorPosition) {
    TerminalEmulator emulator(20, 5);
    emulator.feed("\033[2;4H\033[6n");
    EXPECT_EQ(emulator.takePendingResponse(), "\033[2;4R");
    EXPECT_EQ(emulator.takePendingResponse(), "");
}

TEST(TerminalEmulatorTest, InsertAndDeleteCharacters) {
    TerminalEmulator emulator(10, 2);
    emulator.feed("abcd\033[1;1H\033[2@");
    EXPECT_EQ(firstLine(emulator), "  abcd");
    emulator.feed("\033[2P");
    EXPECT_EQ(firstLine(emulator), "abcd");
}

TEST(TerminalEmulatorTest, Utf8Characters) {
    TerminalEmulator emulator(10, 2);
    emulator.feed("ñ€");
    EXPECT_EQ(emulator.screen()[0][0].character, U'ñ');
    EXPECT_EQ(emulator.screen()[0][1].character, U'€');
    EXPECT_EQ(emulator.cursorColumn(), 2);
}

TEST(TerminalEmulatorTest, ResizeKeepsContent) {
    TerminalEmulator emulator(10, 3);
    emulator.feed("keep");
    emulator.resize(20, 5);
    EXPECT_EQ(firstLine(emulator), "keep");
    EXPECT_EQ(emulator.columns(), 20);
    EXPECT_EQ(emulator.rows(), 5);
}
