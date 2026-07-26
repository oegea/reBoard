#include <string>
#include <vector>

#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QTranslator>

#include "StoreViewModel.h"
#include "application/UseCaseFactory.h"

namespace {

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

std::vector<std::string> manifestDirectories() {
    std::vector<std::string> directories = {"/etc/reboard/apps", "/opt/etc/reboard/apps"};
    const QString home = qEnvironmentVariable("HOME");
    if (!home.isEmpty()) {
        directories.push_back(home.toStdString() + "/.config/reboard/apps");
    }
    return directories;
}

}  // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard-store");
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

    reboard::application::UseCaseFactory useCases(manifestDirectories());
    reboard::store::StoreViewModel viewModel(useCases);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("storeVm", &viewModel);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Store.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-store: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
