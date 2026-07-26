#include <string>
#include <vector>

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "application/UseCaseFactory.h"
#include "ui/BoardViewModel.h"
#include "ui/LauncherController.h"

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

    reboard::application::UseCaseFactory useCases(manifestDirectories());
    reboard::ui::BoardViewModel board(useCases);
    reboard::ui::LauncherController launcher;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("board", &board);
    engine.rootContext()->setContextProperty("launcher", &launcher);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "reboard-ui: failed to load the QML interface";
        return 1;
    }

    return app.exec();
}
