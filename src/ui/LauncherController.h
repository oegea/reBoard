#pragma once

#include <QObject>
#include <QString>
#include <QTimer>

#include "application/UseCaseFactory.h"

namespace reboard::ui {

// Orchestrates the launcher lifecycle from the UI: launching applications,
// reacting to the home gesture, and noticing when the foreground application
// exits on its own.
class LauncherController : public QObject {
    Q_OBJECT

public:
    explicit LauncherController(application::UseCaseFactory& useCases, QObject* parent = nullptr);

    Q_INVOKABLE void launch(const QString& applicationId);

public slots:
    void onHomeGestureDetected();

signals:
    // The QML window reacts to these: the launcher hides while a third-party
    // application owns the e-paper display and shows itself again on "home".
    void requestShowLauncher();
    void requestHideLauncher();

private:
    void refreshForegroundState();

    application::UseCaseFactory& useCases_;
    QTimer foregroundPollTimer_;
};

}  // namespace reboard::ui
