#include "ClaudeViewModel.h"

#include <QSettings>
#include <QVariantMap>

namespace reboard::claude {

ClaudeViewModel::ClaudeViewModel(QObject* parent) : QObject(parent) {
    const QString home = qEnvironmentVariable("HOME");
    QSettings config(home + "/.config/reboard/claude.conf", QSettings::IniFormat);
    host_ = config.value("host").toString();
    directory_ = config.value("directory").toString();

    connect(&session_, &ClaudeSession::eventReceived, this,
            [this](const QVariantMap& event) { onEvent(event); });
    connect(&session_, &ClaudeSession::failed, this, [this](const QString& reason) {
        appendEntry(QStringLiteral("system"), reason);
        setStatus(tr("Connection failed"), false);
    });
    connect(&session_, &ClaudeSession::finished, this,
            [this] { setStatus(tr("Session ended"), false); });

    if (configured()) {
        connectSession();
    }
}

void ClaudeViewModel::connectSession() {
    if (!configured() || session_.running()) {
        return;
    }
    setStatus(tr("Connecting to %1…").arg(host_), true);
    session_.start(host_, directory_);
}

void ClaudeViewModel::sendPrompt(const QString& text) {
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty() || !session_.running()) {
        return;
    }
    appendEntry(QStringLiteral("user"), trimmed);
    setStatus(tr("Working…"), true);
    session_.sendPrompt(trimmed);
}

void ClaudeViewModel::onEvent(const QVariantMap& event) {
    const QString type = event.value("type").toString();

    if (type == QLatin1String("system")) {
        if (event.value("subtype").toString() == QLatin1String("init")) {
            setStatus(tr("Ready"), false);
        }
        return;
    }
    if (type == QLatin1String("assistant")) {
        const QVariantList content =
            event.value("message").toMap().value("content").toList();
        for (const QVariant& blockVariant : content) {
            const QVariantMap block = blockVariant.toMap();
            const QString blockType = block.value("type").toString();
            if (blockType == QLatin1String("text")) {
                appendEntry(QStringLiteral("assistant"), block.value("text").toString());
            } else if (blockType == QLatin1String("tool_use")) {
                appendEntry(QStringLiteral("tool"),
                            tr("Using tool: %1").arg(block.value("name").toString()));
            }
        }
        return;
    }
    if (type == QLatin1String("result")) {
        setStatus(tr("Ready"), false);
    }
}

void ClaudeViewModel::appendEntry(const QString& role, const QString& text) {
    if (text.trimmed().isEmpty()) {
        return;
    }
    QVariantMap entry;
    entry.insert("role", role);
    entry.insert("text", text);
    transcript_.append(entry);
    emit transcriptChanged();
}

void ClaudeViewModel::setStatus(const QString& status, bool busy) {
    status_ = status;
    busy_ = busy;
    emit statusChanged();
}

}  // namespace reboard::claude
