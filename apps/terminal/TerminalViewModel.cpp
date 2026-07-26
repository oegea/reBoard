#include "TerminalViewModel.h"

#include <QChar>

#include "infrastructure/input/EvdevKeyboardDetector.h"

namespace reboard::terminal {

namespace {

QString escapeHtml(const QString& text) {
    QString escaped = text;
    escaped.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    escaped.replace(QLatin1Char('<'), QLatin1String("&lt;"));
    escaped.replace(QLatin1Char('>'), QLatin1String("&gt;"));
    escaped.replace(QLatin1Char(' '), QLatin1String("&nbsp;"));
    return escaped;
}

}  // namespace

TerminalViewModel::TerminalViewModel(QObject* parent) : QObject(parent) {
    // Coalesce PTY bursts into one screen update: far fewer e-paper
    // refreshes without feeling laggy.
    coalesceTimer_.setSingleShot(true);
    coalesceTimer_.setInterval(40);
    connect(&coalesceTimer_, &QTimer::timeout, this, &TerminalViewModel::refreshLines);
    connect(&pty_, &PtySession::outputReceived, this,
            [this](const QByteArray& bytes) { onOutput(bytes); });
    connect(&pty_, &PtySession::finished, this, &TerminalViewModel::refreshLines);
}

bool TerminalViewModel::physicalKeyboardPresent() const {
    return reboard::infrastructure::EvdevKeyboardDetector::keyboardPresent();
}

int TerminalViewModel::cursorRow() const { return emulator_.cursorRow(); }
int TerminalViewModel::cursorColumn() const { return emulator_.cursorColumn(); }
bool TerminalViewModel::cursorVisible() const { return emulator_.cursorVisible(); }

void TerminalViewModel::startShell(int columns, int rows) {
    if (pty_.running()) {
        return;
    }
    emulator_.resize(columns, rows);
    pty_.start(columns, rows);
    refreshLines();
}

void TerminalViewModel::setGridSize(int columns, int rows) {
    if (columns == emulator_.columns() && rows == emulator_.rows()) {
        return;
    }
    emulator_.resize(columns, rows);
    pty_.resize(columns, rows);
    refreshLines();
}

void TerminalViewModel::sendText(const QString& text) {
    if (!text.isEmpty()) {
        pty_.write(text.toUtf8());
    }
}

void TerminalViewModel::sendKey(int qtKey, int modifiers, const QString& text) {
    const bool control = (modifiers & Qt::ControlModifier) != 0;
    QByteArray bytes;

    switch (qtKey) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            bytes = "\r";
            break;
        case Qt::Key_Backspace:
            bytes = "\x7f";
            break;
        case Qt::Key_Tab:
            bytes = "\t";
            break;
        case Qt::Key_Escape:
            bytes = "\033";
            break;
        case Qt::Key_Up:
            bytes = "\033[A";
            break;
        case Qt::Key_Down:
            bytes = "\033[B";
            break;
        case Qt::Key_Right:
            bytes = "\033[C";
            break;
        case Qt::Key_Left:
            bytes = "\033[D";
            break;
        case Qt::Key_Home:
            bytes = "\033[H";
            break;
        case Qt::Key_End:
            bytes = "\033[F";
            break;
        case Qt::Key_PageUp:
            bytes = "\033[5~";
            break;
        case Qt::Key_PageDown:
            bytes = "\033[6~";
            break;
        case Qt::Key_Delete:
            bytes = "\033[3~";
            break;
        default:
            if (control && qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
                bytes = QByteArray(1, static_cast<char>(qtKey - Qt::Key_A + 1));
            } else if (!text.isEmpty()) {
                bytes = text.toUtf8();
            }
            break;
    }

    if (!bytes.isEmpty()) {
        pty_.write(bytes);
    }
}

void TerminalViewModel::onOutput(const QByteArray& bytes) {
    emulator_.feed(std::string(bytes.constData(), static_cast<std::size_t>(bytes.size())));
    const std::string response = emulator_.takePendingResponse();
    if (!response.empty()) {
        pty_.write(QByteArray(response.data(), static_cast<int>(response.size())));
    }
    if (!coalesceTimer_.isActive()) {
        coalesceTimer_.start();
    }
}

void TerminalViewModel::refreshLines() {
    QStringList rendered;
    rendered.reserve(emulator_.rows());

    for (const auto& row : emulator_.screen()) {
        QString html;
        bool bold = false;
        bool inverse = false;
        for (const Cell& cell : row) {
            if (cell.bold != bold) {
                html += cell.bold ? QLatin1String("<b>") : QLatin1String("</b>");
                bold = cell.bold;
            }
            if (cell.inverse != inverse) {
                html += cell.inverse
                            ? QLatin1String(
                                  "<span style=\"background-color:black;color:white;\">")
                            : QLatin1String("</span>");
                inverse = cell.inverse;
            }
            html += escapeHtml(QString::fromUcs4(&cell.character, 1));
        }
        if (inverse) {
            html += QLatin1String("</span>");
        }
        if (bold) {
            html += QLatin1String("</b>");
        }
        rendered.append(html);
    }

    lines_ = rendered;
    emit screenChanged();
}

}  // namespace reboard::terminal
