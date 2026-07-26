#include "LauncherController.h"

#include <iostream>

#include <QCoreApplication>

namespace reboard::ui {

LauncherController::LauncherController(QObject* parent) : QObject(parent) {}

void LauncherController::launch(const QString& applicationId) {
    std::cout << "launch:" << applicationId.toStdString() << std::endl;
    QCoreApplication::quit();
}

}  // namespace reboard::ui
