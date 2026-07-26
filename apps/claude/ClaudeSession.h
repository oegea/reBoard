#pragma once

#include <QByteArray>
#include <QProcess>
#include <QVariantMap>

namespace reboard::claude {

// A remote Claude Code session: runs `claude -p --input-format stream-json
// --output-format stream-json` on the configured host over an SSH exec
// channel and exchanges typed NDJSON events. This is the official
// structured interface — no terminal scraping.
class ClaudeSession : public QObject {
    Q_OBJECT

public:
    explicit ClaudeSession(QObject* parent = nullptr);

    // `host` like user@machine; `directory` is the remote project path.
    void start(const QString& host, const QString& directory);
    void sendPrompt(const QString& text);
    void stop();
    bool running() const { return process_.state() != QProcess::NotRunning; }

signals:
    void eventReceived(const QVariantMap& event);
    void failed(const QString& reason);
    void finished();

private:
    void onReadyRead();

    QProcess process_;
    QByteArray lineBuffer_;
};

}  // namespace reboard::claude
