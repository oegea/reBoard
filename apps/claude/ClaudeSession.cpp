#include "ClaudeSession.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace reboard::claude {

ClaudeSession::ClaudeSession(QObject* parent) : QObject(parent) {
    process_.setProcessChannelMode(QProcess::SeparateChannels);
    connect(&process_, &QProcess::readyReadStandardOutput, this, [this] { onReadyRead(); });
    connect(&process_, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        emit failed(process_.errorString());
    });
    connect(&process_, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            const QString stderrTail =
                QString::fromUtf8(process_.readAllStandardError()).right(400);
            emit failed(stderrTail.isEmpty() ? tr("The remote session ended with an error.")
                                             : stderrTail);
        }
        emit finished();
    });
}

void ClaudeSession::start(const QString& host, const QString& directory) {
    if (running()) {
        return;
    }
    lineBuffer_.clear();
    // BatchMode: fail fast instead of prompting for a password (key-based
    // auth is a phase-1 requirement).
    const QString remoteCommand =
        QStringLiteral("cd %1 && claude -p --input-format stream-json "
                       "--output-format stream-json --verbose")
            .arg(directory.isEmpty() ? QStringLiteral("~") : directory);
    process_.start(QStringLiteral("ssh"),
                   {QStringLiteral("-o"), QStringLiteral("BatchMode=yes"), host, remoteCommand});
}

void ClaudeSession::sendPrompt(const QString& text) {
    if (!running()) {
        return;
    }
    QJsonObject content{{QStringLiteral("type"), QStringLiteral("text")},
                        {QStringLiteral("text"), text}};
    QJsonObject message{{QStringLiteral("role"), QStringLiteral("user")},
                        {QStringLiteral("content"), QJsonArray{content}}};
    QJsonObject event{{QStringLiteral("type"), QStringLiteral("user")},
                      {QStringLiteral("message"), message}};
    process_.write(QJsonDocument(event).toJson(QJsonDocument::Compact) + "\n");
}

void ClaudeSession::stop() {
    if (running()) {
        process_.closeWriteChannel();
        process_.terminate();
    }
}

void ClaudeSession::onReadyRead() {
    lineBuffer_ += process_.readAllStandardOutput();
    int newline;
    while ((newline = lineBuffer_.indexOf('\n')) >= 0) {
        const QByteArray line = lineBuffer_.left(newline).trimmed();
        lineBuffer_.remove(0, newline + 1);
        if (line.isEmpty()) {
            continue;
        }
        const QJsonDocument document = QJsonDocument::fromJson(line);
        if (document.isObject()) {
            emit eventReceived(document.object().toVariantMap());
        }
    }
}

}  // namespace reboard::claude
