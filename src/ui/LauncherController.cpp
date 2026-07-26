#include "ui/LauncherController.h"

#include <QDebug>

namespace reboard::ui {

namespace {
constexpr int kForegroundPollIntervalMs = 2000;
}

LauncherController::LauncherController(application::UseCaseFactory& useCases, QObject* parent)
    : QObject(parent), useCases_(useCases) {
    connect(&foregroundPollTimer_, &QTimer::timeout, this,
            &LauncherController::refreshForegroundState);
    foregroundPollTimer_.start(kForegroundPollIntervalMs);
}

void LauncherController::launch(const QString& applicationId) {
    try {
        const auto result = useCases_.launchApplication().execute(
            domain::ApplicationId(applicationId.toStdString()));
        if (result == application::LaunchResult::Launched) {
            emit requestHideLauncher();
        } else {
            qWarning() << "reboard: unknown application" << applicationId;
        }
    } catch (const std::exception& exception) {
        qWarning() << "reboard: failed to launch" << applicationId << ":" << exception.what();
    }
}

void LauncherController::onHomeGestureDetected() {
    try {
        useCases_.closeForegroundApplication().execute();
    } catch (const std::exception& exception) {
        qWarning() << "reboard: failed to close the foreground application:" << exception.what();
    }
    emit requestShowLauncher();
}

void LauncherController::refreshForegroundState() {
    try {
        const auto state = useCases_.refreshForegroundState().execute();
        if (state == application::ForegroundState::Exited) {
            emit requestShowLauncher();
        }
    } catch (const std::exception& exception) {
        qWarning() << "reboard: failed to refresh the foreground state:" << exception.what();
    }
}

}  // namespace reboard::ui
