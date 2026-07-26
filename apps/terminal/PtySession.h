#pragma once

#include <QByteArray>
#include <QObject>
#include <QSocketNotifier>

namespace reboard::terminal {

// Owns the PTY and the shell child process. Emits raw output bytes; input
// is written verbatim. Pure plumbing — all terminal intelligence lives in
// TerminalEmulator.
class PtySession : public QObject {
    Q_OBJECT

public:
    explicit PtySession(QObject* parent = nullptr);
    ~PtySession() override;

    bool start(int columns, int rows);
    void write(const QByteArray& bytes);
    void resize(int columns, int rows);
    bool running() const { return childPid_ > 0; }

signals:
    void outputReceived(const QByteArray& bytes);
    void finished();

private:
    void onReadable();

    int masterFd_ = -1;
    int childPid_ = -1;
    QSocketNotifier* notifier_ = nullptr;
};

}  // namespace reboard::terminal
