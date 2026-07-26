#include "LauncherController.h"

#include <iostream>

#include <QCoreApplication>
#include <QDebug>

namespace reboard::ui {

LauncherController::LauncherController(application::UseCaseFactory& useCases, QObject* parent)
    : QObject(parent), useCases_(useCases) {}

void LauncherController::launch(const QString& applicationId) {
    std::cout << "launch:" << applicationId.toStdString() << std::endl;
    QCoreApplication::quit();
}

bool LauncherController::uninstall(const QString& applicationId) {
    try {
        useCases_.uninstallApplication().execute(
            domain::ApplicationId(applicationId.toStdString()));
        return true;
    } catch (const std::exception& exception) {
        qWarning() << "reboard-ui: failed to uninstall" << applicationId << ":"
                   << exception.what();
        return false;
    }
}

}  // namespace reboard::ui
