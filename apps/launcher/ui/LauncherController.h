#pragma once

#include <QObject>
#include <QString>

#include "application/UseCaseFactory.h"

namespace reboard::ui {

// The UI is a short-lived process: choosing an application prints a
// directive to stdout and quits. The resident daemon (which spawned us)
// launches the application once this process is dead and the e-paper
// display is free.
class LauncherController : public QObject {
    Q_OBJECT

public:
    explicit LauncherController(application::UseCaseFactory& useCases, QObject* parent = nullptr);

    Q_INVOKABLE void launch(const QString& applicationId);

    // Store-app removal (ADR-0006): the package repository only touches the
    // store-managed locations, so base apps are safe by construction.
    Q_INVOKABLE bool uninstall(const QString& applicationId);

private:
    application::UseCaseFactory& useCases_;
};

}  // namespace reboard::ui
