#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace reboard::terminal {

// One character cell. Colors are intentionally reduced to what e-paper can
// express: bold and inverse video.
struct Cell {
    char32_t character = U' ';
    bool bold = false;
    bool inverse = false;

    bool operator==(const Cell& other) const {
        return character == other.character && bold == other.bold && inverse == other.inverse;
    }
};

// A VT100/xterm-subset terminal emulator, deliberately free of any UI or OS
// dependency so it is fully unit-testable. It consumes raw bytes from the
// PTY and maintains the screen grid; responses that must be written back to
// the application (cursor position reports, device attributes) accumulate
// in `pendingResponse`.
//
// Supported: UTF-8, C0 controls, CSI cursor movement/erase/insert/delete,
// scroll regions, SGR (bold/inverse; colors accepted and ignored), the
// alternate screen (1049/47), autowrap with deferred wrap, save/restore
// cursor, and a scrollback for the primary screen.
class TerminalEmulator {
public:
    TerminalEmulator(int columns, int rows, int scrollbackLimit = 2000);

    void feed(const std::string& bytes);
    void resize(int columns, int rows);

    int columns() const { return columns_; }
    int rows() const { return rows_; }
    int cursorRow() const { return cursorRow_; }
    int cursorColumn() const { return cursorColumn_; }
    bool cursorVisible() const { return cursorVisible_; }
    bool onAlternateScreen() const { return usingAlternate_; }

    const std::vector<std::vector<Cell>>& screen() const {
        return usingAlternate_ ? alternate_ : primary_;
    }
    const std::deque<std::vector<Cell>>& scrollback() const { return scrollback_; }

    // Bytes the host must write back to the PTY (DSR/DA replies).
    std::string takePendingResponse();

    // Plain-text dump of the visible screen (testing/debugging).
    std::string screenAsText() const;

private:
    enum class ParseState { Ground, Escape, Csi, Osc, Charset };

    std::vector<std::vector<Cell>>& buffer() { return usingAlternate_ ? alternate_ : primary_; }
    std::vector<Cell> blankLine() const;

    void putCharacter(char32_t character);
    void lineFeed();
    void reverseLineFeed();
    void carriageReturn();
    void backspace();
    void horizontalTab();
    void scrollUp(int count);
    void scrollDown(int count);
    void handleEscape(char byte);
    void handleCsi(char final);
    void handleSgr();
    void setMode(bool enable);
    void eraseInDisplay(int mode);
    void eraseInLine(int mode);
    void clampCursor();
    int param(std::size_t index, int fallback) const;

    // UTF-8 decoding state.
    void feedByte(unsigned char byte);
    char32_t pendingCodepoint_ = 0;
    int pendingUtf8Bytes_ = 0;

    int columns_;
    int rows_;
    int scrollbackLimit_;

    std::vector<std::vector<Cell>> primary_;
    std::vector<std::vector<Cell>> alternate_;
    std::deque<std::vector<Cell>> scrollback_;
    bool usingAlternate_ = false;

    int cursorRow_ = 0;
    int cursorColumn_ = 0;
    int savedRow_ = 0;
    int savedColumn_ = 0;
    bool cursorVisible_ = true;
    bool autowrap_ = true;
    bool pendingWrap_ = false;

    int scrollTop_ = 0;     // Inclusive.
    int scrollBottom_ = 0;  // Inclusive.

    bool attributeBold_ = false;
    bool attributeInverse_ = false;

    ParseState state_ = ParseState::Ground;
    std::string csiParams_;
    bool csiPrivate_ = false;
    std::string oscBuffer_;
    std::string pendingResponse_;
};

}  // namespace reboard::terminal
