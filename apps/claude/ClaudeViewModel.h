#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

#include "ClaudeSession.h"

namespace reboard::claude {

// Drives the Claude Code companion screen: transcript entries, session
// state and prompt sending. Configuration (phase 1) comes from
// ~/.config/reboard/claude.conf ([General] host=user@machine,
// directory=/path/to/project) until the in-app setup lands (story 010).
class ClaudeViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList transcript READ transcript NOTIFY transcriptChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool configured READ configured CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY statusChanged)
    Q_PROPERTY(QString host READ host CONSTANT)

public:
    explicit ClaudeViewModel(QObject* parent = nullptr);

    QVariantList transcript() const { return transcript_; }
    QString status() const { return status_; }
    bool configured() const { return !host_.isEmpty(); }
    bool busy() const { return busy_; }
    QString host() const { return host_; }

    Q_INVOKABLE void connectSession();
    Q_INVOKABLE void sendPrompt(const QString& text);

signals:
    void transcriptChanged();
    void statusChanged();

private:
    void onEvent(const QVariantMap& event);
    void appendEntry(const QString& role, const QString& text);
    void setStatus(const QString& status, bool busy);

    ClaudeSession session_;
    QVariantList transcript_;
    QString status_;
    QString host_;
    QString directory_;
    bool busy_ = false;
};

}  // namespace reboard::claude
