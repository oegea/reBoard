#include <string>
#include <vector>

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "application/UseCaseFactory.h"
#include "ui/BoardViewModel.h"
#include "ui/LauncherController.h"
#include "ui/TouchGestureMonitor.h"

namespace {

std::vector<std::string> manifestDirectories() {
    std::vector<std::string> directories = {
        "/etc/reboard/apps",
        "/opt/etc/reboard/apps",
    };
    const QString home = qEnvironmentVariable("HOME");
    if (!home.isEmpty()) {
        directories.push_back(home.toStdString() + "/.config/reboard/apps");
    }
    return directories;
}

}  // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard");
    QGuiApplication::setApplicationVersion(REBOARD_VERSION);

    reboard::application::UseCaseFactory useCases(manifestDirectories());

    // If the stock UI (or another unit-based application) is already on
    // screen, adopt it so the home gesture can close it.
    try {
        useCases.adoptRunningApplication().execute();
    } catch (const std::exception& exception) {
        qWarning() << "reboard: could not adopt the running application:" << exception.what();
    }

    reboard::ui::BoardViewModel board(useCases);
    reboard::ui::LauncherController launcher(useCases);

    // The touch transform differs per device; reMarkable 2 needs the Y axis
    // inverted. Overridable for other devices without recompiling.
    const bool invertX = qEnvironmentVariableIsSet("REBOARD_TOUCH_INVERT_X");
    const bool invertY = !qEnvironmentVariableIsSet("REBOARD_TOUCH_NO_INVERT_Y");
    reboard::ui::TouchGestureMonitor gestureMonitor(
        qEnvironmentVariable("REBOARD_TOUCH_DEVICE").toStdString(), invertX, invertY);
    QObject::connect(&gestureMonitor, &reboard::ui::TouchGestureMonitor::homeGestureDetected,
                     &launcher, &reboard::ui::LauncherController::onHomeGestureDetected);
    gestureMonitor.start();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("board", &board);
    engine.rootContext()->setContextProperty("launcher", &launcher);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard: failed to load the QML interface";
        return 1;
    }

    const int exitCode = app.exec();
    gestureMonitor.requestStop();
    gestureMonitor.wait(2000);
    return exitCode;
}
