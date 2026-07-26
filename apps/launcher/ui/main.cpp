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

    // Translations (ADR-0004): env override > persisted choice > system.
    QTranslator translator;
    QLocale locale = QLocale::system();
    const QString localeOverride = qEnvironmentVariable("REBOARD_LOCALE");
    if (!localeOverride.isEmpty()) {
        locale = QLocale(localeOverride);
    } else {
        QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
        const QString language = config.value("language", "system").toString();
        if (language != QStringLiteral("system") && !language.isEmpty()) {
            locale = QLocale(language);
        }
    }
    if (translator.load(locale, "reboard", "_", ":/i18n")) {
        QGuiApplication::installTranslator(&translator);
    }

    reboard::application::UseCaseFactory useCases(manifestDirectories());
    reboard::ui::BoardViewModel board(useCases);
    reboard::ui::LauncherController launcher;

    // Content rotation (0, 90 or 270) decided by the daemon; see
    // docs/stories/001-orientation-detection.md.
    bool rotationParsed = false;
    int uiRotation = qEnvironmentVariableIntValue("REBOARD_UI_ROTATION", &rotationParsed);
    if (!rotationParsed || (uiRotation != 90 && uiRotation != 270)) {
        uiRotation = 0;
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("board", &board);
    engine.rootContext()->setContextProperty("launcher", &launcher);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-ui: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
