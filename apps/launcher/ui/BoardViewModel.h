#pragma once

#include <QObject>
#include <QVariantList>

#include "application/UseCaseFactory.h"

namespace reboard::ui {

// Exposes the board (pages + dock) to QML as plain variant lists.
class BoardViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList pages READ pages NOTIFY boardChanged)
    Q_PROPERTY(QVariantList dock READ dock NOTIFY boardChanged)

public:
    explicit BoardViewModel(application::UseCaseFactory& useCases, QObject* parent = nullptr);

    QVariantList pages() const { return pages_; }
    QVariantList dock() const { return dock_; }

    Q_INVOKABLE void reload();

signals:
    void boardChanged();

private:
    application::UseCaseFactory& useCases_;
    QVariantList pages_;
    QVariantList dock_;
};

}  // namespace reboard::ui
