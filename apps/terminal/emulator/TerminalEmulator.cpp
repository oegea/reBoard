#include "TerminalEmulator.h"

#include <algorithm>
#include <sstream>

namespace reboard::terminal {

TerminalEmulator::TerminalEmulator(int columns, int rows, int scrollbackLimit)
    : columns_(std::max(1, columns)),
      rows_(std::max(1, rows)),
      scrollbackLimit_(scrollbackLimit),
      scrollBottom_(rows_ - 1) {
    primary_.assign(rows_, blankLine());
    alternate_.assign(rows_, blankLine());
}

std::vector<Cell> TerminalEmulator::blankLine() const {
    return std::vector<Cell>(static_cast<std::size_t>(columns_), Cell{});
}

void TerminalEmulator::feed(const std::string& bytes) {
    for (const char byte : bytes) {
        feedByte(static_cast<unsigned char>(byte));
    }
}

void TerminalEmulator::feedByte(unsigned char byte) {
    // UTF-8 continuation handling happens before control parsing.
    if (pendingUtf8Bytes_ > 0) {
        if ((byte & 0xC0) == 0x80) {
            pendingCodepoint_ = (pendingCodepoint_ << 6) | (byte & 0x3F);
            if (--pendingUtf8Bytes_ == 0 && state_ == ParseState::Ground) {
                putCharacter(pendingCodepoint_);
            }
            return;
        }
        pendingUtf8Bytes_ = 0;  // Malformed sequence: drop and reprocess.
    }

    switch (state_) {
        case ParseState::Ground:
            if (byte == 0x1B) {
                state_ = ParseState::Escape;
            } else if (byte == '\n' || byte == 0x0B || byte == 0x0C) {
                lineFeed();
            } else if (byte == '\r') {
                carriageReturn();
            } else if (byte == 0x08) {
                backspace();
            } else if (byte == '\t') {
                horizontalTab();
            } else if (byte == 0x07) {
                // BEL: nothing to ring on e-paper.
            } else if (byte < 0x20 || byte == 0x7F) {
                // Other C0 controls: ignored.
            } else if (byte < 0x80) {
                putCharacter(byte);
            } else if ((byte & 0xE0) == 0xC0) {
                pendingCodepoint_ = byte & 0x1F;
                pendingUtf8Bytes_ = 1;
            } else if ((byte & 0xF0) == 0xE0) {
                pendingCodepoint_ = byte & 0x0F;
                pendingUtf8Bytes_ = 2;
            } else if ((byte & 0xF8) == 0xF0) {
                pendingCodepoint_ = byte & 0x07;
                pendingUtf8Bytes_ = 3;
            }
            break;

        case ParseState::Escape:
            handleEscape(static_cast<char>(byte));
            break;

        case ParseState::Csi:
            if ((byte >= '0' && byte <= '9') || byte == ';') {
                csiParams_.push_back(static_cast<char>(byte));
            } else if (byte == '?') {
                csiPrivate_ = true;
            } else if (byte >= 0x40 && byte <= 0x7E) {
                handleCsi(static_cast<char>(byte));
                state_ = ParseState::Ground;
            }
            // Intermediate bytes (space, '>', '!') are consumed silently.
            break;

        case ParseState::Osc:
            if (byte == 0x07) {
                state_ = ParseState::Ground;  // BEL terminates OSC.
            } else if (byte == 0x1B) {
                state_ = ParseState::Escape;  // ESC \ terminator: the '\'
                                              // will be consumed as escape.
            } else {
                oscBuffer_.push_back(static_cast<char>(byte));
            }
            break;

        case ParseState::Charset:
            state_ = ParseState::Ground;  // Designator consumed, ignored.
            break;
    }
}

void TerminalEmulator::handleEscape(char byte) {
    state_ = ParseState::Ground;
    switch (byte) {
        case '[':
            state_ = ParseState::Csi;
            csiParams_.clear();
            csiPrivate_ = false;
            break;
        case ']':
            state_ = ParseState::Osc;
            oscBuffer_.clear();
            break;
        case '(':
        case ')':
            state_ = ParseState::Charset;
            break;
        case '7':
            savedRow_ = cursorRow_;
            savedColumn_ = cursorColumn_;
            break;
        case '8':
            cursorRow_ = savedRow_;
            cursorColumn_ = savedColumn_;
            pendingWrap_ = false;
            clampCursor();
            break;
        case 'D':
            lineFeed();
            break;
        case 'E':
            lineFeed();
            carriageReturn();
            break;
        case 'M':
            reverseLineFeed();
            break;
        case 'c': {  // Full reset.
            primary_.assign(rows_, blankLine());
            alternate_.assign(rows_, blankLine());
            scrollback_.clear();
            cursorRow_ = cursorColumn_ = 0;
            scrollTop_ = 0;
            scrollBottom_ = rows_ - 1;
            attributeBold_ = attributeInverse_ = false;
            usingAlternate_ = false;
            cursorVisible_ = true;
            pendingWrap_ = false;
            break;
        }
        case '\\':
            break;  // String terminator (ends an OSC).
        default:
            break;
    }
}

int TerminalEmulator::param(std::size_t index, int fallback) const {
    std::size_t current = 0;
    std::string value;
    for (const char c : csiParams_) {
        if (c == ';') {
            if (current == index) {
                break;
            }
            ++current;
            value.clear();
        } else if (current == index) {
            value.push_back(c);
        }
    }
    if (current < index || value.empty()) {
        return fallback;
    }
    return std::stoi(value);
}

void TerminalEmulator::handleCsi(char final) {
    pendingWrap_ = false;
    switch (final) {
        case 'A':
            cursorRow_ -= param(0, 1);
            break;
        case 'B':
            cursorRow_ += param(0, 1);
            break;
        case 'C':
            cursorColumn_ += param(0, 1);
            break;
        case 'D':
            cursorColumn_ -= param(0, 1);
            break;
        case 'E':
            cursorRow_ += param(0, 1);
            cursorColumn_ = 0;
            break;
        case 'F':
            cursorRow_ -= param(0, 1);
            cursorColumn_ = 0;
            break;
        case 'G':
            cursorColumn_ = param(0, 1) - 1;
            break;
        case 'H':
        case 'f':
            cursorRow_ = param(0, 1) - 1;
            cursorColumn_ = param(1, 1) - 1;
            break;
        case 'd':
            cursorRow_ = param(0, 1) - 1;
            break;
        case 'J':
            eraseInDisplay(param(0, 0));
            break;
        case 'K':
            eraseInLine(param(0, 0));
            break;
        case 'L': {  // Insert lines at the cursor (within the region).
            const int count = param(0, 1);
            auto& lines = buffer();
            for (int i = 0; i < count && cursorRow_ <= scrollBottom_; ++i) {
                lines.erase(lines.begin() + scrollBottom_);
                lines.insert(lines.begin() + cursorRow_, blankLine());
            }
            break;
        }
        case 'M': {  // Delete lines at the cursor (within the region).
            const int count = param(0, 1);
            auto& lines = buffer();
            for (int i = 0; i < count && cursorRow_ <= scrollBottom_; ++i) {
                lines.erase(lines.begin() + cursorRow_);
                lines.insert(lines.begin() + scrollBottom_, blankLine());
            }
            break;
        }
        case '@': {  // Insert blank characters.
            const int count = param(0, 1);
            auto& line = buffer()[static_cast<std::size_t>(cursorRow_)];
            for (int i = 0; i < count; ++i) {
                line.insert(line.begin() + cursorColumn_, Cell{});
                line.pop_back();
            }
            break;
        }
        case 'P': {  // Delete characters.
            const int count = param(0, 1);
            auto& line = buffer()[static_cast<std::size_t>(cursorRow_)];
            for (int i = 0; i < count && cursorColumn_ < columns_; ++i) {
                line.erase(line.begin() + cursorColumn_);
                line.push_back(Cell{});
            }
            break;
        }
        case 'X': {  // Erase characters.
            const int count = param(0, 1);
            auto& line = buffer()[static_cast<std::size_t>(cursorRow_)];
            for (int i = 0; i < count && cursorColumn_ + i < columns_; ++i) {
                line[static_cast<std::size_t>(cursorColumn_ + i)] = Cell{};
            }
            break;
        }
        case 'S':
            scrollUp(param(0, 1));
            break;
        case 'T':
            scrollDown(param(0, 1));
            break;
        case 'r':
            scrollTop_ = param(0, 1) - 1;
            scrollBottom_ = param(1, rows_) - 1;
            scrollTop_ = std::clamp(scrollTop_, 0, rows_ - 1);
            scrollBottom_ = std::clamp(scrollBottom_, scrollTop_, rows_ - 1);
            cursorRow_ = cursorColumn_ = 0;
            break;
        case 'm':
            handleSgr();
            break;
        case 'h':
            setMode(true);
            break;
        case 'l':
            setMode(false);
            break;
        case 's':
            savedRow_ = cursorRow_;
            savedColumn_ = cursorColumn_;
            break;
        case 'u':
            cursorRow_ = savedRow_;
            cursorColumn_ = savedColumn_;
            break;
        case 'n':
            if (param(0, 0) == 6) {  // DSR: report cursor position.
                std::ostringstream report;
                report << "\033[" << (cursorRow_ + 1) << ';' << (cursorColumn_ + 1) << 'R';
                pendingResponse_ += report.str();
            }
            break;
        case 'c':
            pendingResponse_ += "\033[?6c";  // DA: VT102.
            break;
        default:
            break;  // Unsupported finals are ignored on purpose.
    }
    clampCursor();
}

void TerminalEmulator::handleSgr() {
    if (csiParams_.empty()) {
        attributeBold_ = attributeInverse_ = false;
        return;
    }
    for (std::size_t index = 0;; ++index) {
        const int code = param(index, -1);
        if (code == -1 && index > 0) {
            break;
        }
        switch (code) {
            case -1:
            case 0:
                attributeBold_ = attributeInverse_ = false;
                break;
            case 1:
                attributeBold_ = true;
                break;
            case 7:
                attributeInverse_ = true;
                break;
            case 22:
                attributeBold_ = false;
                break;
            case 27:
                attributeInverse_ = false;
                break;
            default:
                break;  // Colors and the rest: accepted, ignored.
        }
        if (code == -1) {
            break;
        }
    }
}

void TerminalEmulator::setMode(bool enable) {
    const int mode = param(0, 0);
    if (!csiPrivate_) {
        return;
    }
    switch (mode) {
        case 25:
            cursorVisible_ = enable;
            break;
        case 7:
            autowrap_ = enable;
            break;
        case 47:
        case 1047:
        case 1049:
            if (enable && !usingAlternate_) {
                savedRow_ = cursorRow_;
                savedColumn_ = cursorColumn_;
                alternate_.assign(rows_, blankLine());
                usingAlternate_ = true;
                cursorRow_ = cursorColumn_ = 0;
            } else if (!enable && usingAlternate_) {
                usingAlternate_ = false;
                cursorRow_ = savedRow_;
                cursorColumn_ = savedColumn_;
            }
            break;
        default:
            break;
    }
}

void TerminalEmulator::putCharacter(char32_t character) {
    if (pendingWrap_ && autowrap_) {
        pendingWrap_ = false;
        carriageReturn();
        lineFeed();
    }
    auto& line = buffer()[static_cast<std::size_t>(cursorRow_)];
    Cell& cell = line[static_cast<std::size_t>(cursorColumn_)];
    cell.character = character;
    cell.bold = attributeBold_;
    cell.inverse = attributeInverse_;

    if (cursorColumn_ + 1 >= columns_) {
        pendingWrap_ = true;  // Deferred wrap: move only on the next glyph.
    } else {
        ++cursorColumn_;
    }
}

void TerminalEmulator::lineFeed() {
    if (cursorRow_ == scrollBottom_) {
        scrollUp(1);
    } else if (cursorRow_ < rows_ - 1) {
        ++cursorRow_;
    }
    pendingWrap_ = false;
}

void TerminalEmulator::reverseLineFeed() {
    if (cursorRow_ == scrollTop_) {
        scrollDown(1);
    } else if (cursorRow_ > 0) {
        --cursorRow_;
    }
    pendingWrap_ = false;
}

void TerminalEmulator::carriageReturn() {
    cursorColumn_ = 0;
    pendingWrap_ = false;
}

void TerminalEmulator::backspace() {
    if (cursorColumn_ > 0) {
        --cursorColumn_;
    }
    pendingWrap_ = false;
}

void TerminalEmulator::horizontalTab() {
    const int next = ((cursorColumn_ / 8) + 1) * 8;
    cursorColumn_ = std::min(next, columns_ - 1);
}

void TerminalEmulator::scrollUp(int count) {
    auto& lines = buffer();
    for (int i = 0; i < count; ++i) {
        if (!usingAlternate_ && scrollTop_ == 0) {
            scrollback_.push_back(lines[static_cast<std::size_t>(scrollTop_)]);
            while (scrollback_.size() > static_cast<std::size_t>(scrollbackLimit_)) {
                scrollback_.pop_front();
            }
        }
        lines.erase(lines.begin() + scrollTop_);
        lines.insert(lines.begin() + scrollBottom_, blankLine());
    }
}

void TerminalEmulator::scrollDown(int count) {
    auto& lines = buffer();
    for (int i = 0; i < count; ++i) {
        lines.erase(lines.begin() + scrollBottom_);
        lines.insert(lines.begin() + scrollTop_, blankLine());
    }
}

void TerminalEmulator::eraseInDisplay(int mode) {
    auto& lines = buffer();
    if (mode == 2 || mode == 3) {
        lines.assign(rows_, blankLine());
        return;
    }
    if (mode == 0) {
        eraseInLine(0);
        for (int row = cursorRow_ + 1; row < rows_; ++row) {
            lines[static_cast<std::size_t>(row)] = blankLine();
        }
    } else if (mode == 1) {
        eraseInLine(1);
        for (int row = 0; row < cursorRow_; ++row) {
            lines[static_cast<std::size_t>(row)] = blankLine();
        }
    }
}

void TerminalEmulator::eraseInLine(int mode) {
    auto& line = buffer()[static_cast<std::size_t>(cursorRow_)];
    if (mode == 0) {
        for (int column = cursorColumn_; column < columns_; ++column) {
            line[static_cast<std::size_t>(column)] = Cell{};
        }
    } else if (mode == 1) {
        for (int column = 0; column <= cursorColumn_ && column < columns_; ++column) {
            line[static_cast<std::size_t>(column)] = Cell{};
        }
    } else if (mode == 2) {
        line = blankLine();
    }
}

void TerminalEmulator::clampCursor() {
    cursorRow_ = std::clamp(cursorRow_, 0, rows_ - 1);
    cursorColumn_ = std::clamp(cursorColumn_, 0, columns_ - 1);
}

void TerminalEmulator::resize(int columns, int rows) {
    columns = std::max(1, columns);
    rows = std::max(1, rows);
    for (auto* linesPointer : {&primary_, &alternate_}) {
        auto& lines = *linesPointer;
        lines.resize(static_cast<std::size_t>(rows));
        for (auto& line : lines) {
            line.resize(static_cast<std::size_t>(columns), Cell{});
        }
    }
    columns_ = columns;
    rows_ = rows;
    scrollTop_ = 0;
    scrollBottom_ = rows_ - 1;
    pendingWrap_ = false;
    clampCursor();
}

std::string TerminalEmulator::takePendingResponse() {
    std::string response = std::move(pendingResponse_);
    pendingResponse_.clear();
    return response;
}

std::string TerminalEmulator::screenAsText() const {
    std::string text;
    for (const auto& line : screen()) {
        std::string rendered;
        for (const Cell& cell : line) {
            rendered += cell.character < 0x80 ? std::string(1, static_cast<char>(cell.character))
                                              : std::string("?");
        }
        while (!rendered.empty() && rendered.back() == ' ') {
            rendered.pop_back();
        }
        text += rendered;
        text += '\n';
    }
    return text;
}

}  // namespace reboard::terminal
