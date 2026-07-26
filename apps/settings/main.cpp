#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QTranslator>

#include "LocaleResolver.h"
#include "SettingsViewModel.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard-settings");
    QGuiApplication::setApplicationVersion(REBOARD_VERSION);

    QTranslator translator;
    if (translator.load(reboard::rekit::resolveLocale(), "reboard", "_", ":/i18n")) {
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
