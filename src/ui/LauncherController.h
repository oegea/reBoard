#pragma once

#include <QObject>
#include <QString>

namespace reboard::ui {

// The UI is a short-lived process: choosing an application prints a
// directive to stdout and quits. The resident daemon (which spawned us)
// launches the application once this process is dead and the e-paper
// display is free.
class LauncherController : public QObject {
    Q_OBJECT

public:
    explicit LauncherController(QObject* parent = nullptr);

    Q_INVOKABLE void launch(const QString& applicationId);
};

}  // namespace reboard::ui
