#pragma once

#include <QObject>
#include <QStringList>
#include <QTimer>

#include "PtySession.h"
#include "emulator/TerminalEmulator.h"

namespace reboard::terminal {

// Bridges the emulator/PTY pair to QML. Output is coalesced (e-paper
// friendly: one screen update per batch, not per byte) and rendered as
// rich-text lines (bold + inverse only, per the e-paper palette).
class TerminalViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList lines READ lines NOTIFY screenChanged)
    Q_PROPERTY(int cursorRow READ cursorRow NOTIFY screenChanged)
    Q_PROPERTY(int cursorColumn READ cursorColumn NOTIFY screenChanged)
    Q_PROPERTY(bool cursorVisible READ cursorVisible NOTIFY screenChanged)
    Q_PROPERTY(bool shellRunning READ shellRunning NOTIFY screenChanged)

public:
    explicit TerminalViewModel(QObject* parent = nullptr);

    QStringList lines() const { return lines_; }
    int cursorRow() const;
    int cursorColumn() const;
    bool cursorVisible() const;
    bool shellRunning() const { return pty_.running(); }

    // Called by QML once the grid geometry is known (font metrics).
    Q_INVOKABLE void startShell(int columns, int rows);
    Q_INVOKABLE void setGridSize(int columns, int rows);

    // Text typed via the on-screen keyboard or committed key events.
    Q_INVOKABLE void sendText(const QString& text);
    // Special keys: Qt key code + modifiers, mapped to control sequences.
    Q_INVOKABLE void sendKey(int qtKey, int modifiers, const QString& text);

signals:
    void screenChanged();

private:
    void onOutput(const QByteArray& bytes);
    void refreshLines();

    TerminalEmulator emulator_{80, 24};
    PtySession pty_;
    QTimer coalesceTimer_;
    QStringList lines_;
};

}  // namespace reboard::terminal
