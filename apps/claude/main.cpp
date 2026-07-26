#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "ClaudeViewModel.h"
#include "LocaleResolver.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard-claude");
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

    reboard::claude::ClaudeViewModel viewModel;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("claudeVm", &viewModel);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Claude.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-claude: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
