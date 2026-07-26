#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QTranslator>

#include "SettingsViewModel.h"

namespace {

// Locale resolution per ADR-0004: env override > persisted choice > system.
QLocale resolveLocale() {
    const QString envOverride = qEnvironmentVariable("REBOARD_LOCALE");
    if (!envOverride.isEmpty()) {
        return QLocale(envOverride);
    }
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
    const QString language = config.value("language", "system").toString();
    if (language != QStringLiteral("system") && !language.isEmpty()) {
        return QLocale(language);
    }
    return QLocale::system();
}

}  // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard-settings");
    QGuiApplication::setApplicationVersion(REBOARD_VERSION);

    QTranslator translator;
    if (translator.load(resolveLocale(), "reboard", "_", ":/i18n")) {
        QGuiApplication::installTranslator(&translator);
    }

    bool rotationParsed = false;
    int uiRotation = qEnvironmentVariableIntValue("REBOARD_UI_ROTATION", &rotationParsed);
    if (!rotationParsed || (uiRotation != 90 && uiRotation != 270)) {
        uiRotation = 0;
    }

    reboard::settings::SettingsViewModel viewModel;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("settingsVm", &viewModel);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Settings.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-settings: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
