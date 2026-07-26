#include <string>
#include <vector>

#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QTranslator>

#include "application/UseCaseFactory.h"
#include "BoardViewModel.h"
#include "LauncherController.h"
#include "LocaleResolver.h"
#include "PowerButtonMonitor.h"

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
    QGuiApplication::setApplicationName("reboard-ui");
    QGuiApplication::setApplicationVersion(REBOARD_VERSION);

    // Translations (ADR-0004): shared policy, see rekit LocaleResolver.
    QTranslator translator;
    if (translator.load(reboard::rekit::resolveLocale(), "reboard", "_", ":/i18n")) {
        QGuiApplication::installTranslator(&translator);
    }

    reboard::application::UseCaseFactory useCases(manifestDirectories());
    reboard::ui::BoardViewModel board(useCases);
    reboard::ui::LauncherController launcher(useCases);

    // Content rotation (0, 90 or 270) decided by the daemon; see
    // docs/stories/001-orientation-detection.md.
    bool rotationParsed = false;
    int uiRotation = qEnvironmentVariableIntValue("REBOARD_UI_ROTATION", &rotationParsed);
    if (!rotationParsed || (uiRotation != 90 && uiRotation != 270)) {
        uiRotation = 0;
    }

    // Lock screen trigger (story 007): the launcher owns the power button
    // while the board is on screen (logind ignores it on this device).
    reboard::ui::PowerButtonMonitor powerMonitor(
        qEnvironmentVariable("REBOARD_POWER_DEVICE").toStdString());
    powerMonitor.start();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("board", &board);
    engine.rootContext()->setContextProperty("launcher", &launcher);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.rootContext()->setContextProperty("powerMonitor", &powerMonitor);
    // Set by the daemon when the previous foreground app ended abnormally.
    engine.rootContext()->setContextProperty("crashNoticeApp",
                                             qEnvironmentVariable("REBOARD_CRASH_NOTICE"));
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-ui: failed to load the QML interface";
        return 1;
    }

    const int exitCode = app.exec();
    powerMonitor.requestStop();
    powerMonitor.wait(2000);
    return exitCode;
}
