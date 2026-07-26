#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "LocaleResolver.h"
#include "TerminalViewModel.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("reboard-terminal");
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

    reboard::terminal::TerminalViewModel viewModel;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("terminalVm", &viewModel);
    engine.rootContext()->setContextProperty("uiRotation", uiRotation);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Terminal.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-terminal: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
