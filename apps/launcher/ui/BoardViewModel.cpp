#include "BoardViewModel.h"

#include <QDebug>
#include <QVariantMap>

namespace reboard::ui {

namespace {

QVariantMap toVariant(const domain::Application& application) {
    QVariantMap map;
    map.insert("appId", QString::fromStdString(application.id().value()));
    map.insert("name", QString::fromStdString(application.name().value()));
    map.insert("icon", QString::fromStdString(application.iconPath()));
    const QString name = QString::fromStdString(application.name().value());
    map.insert("initial", name.isEmpty() ? QString("?") : name.left(1).toUpper());
    return map;
}

}  // namespace

BoardViewModel::BoardViewModel(application::UseCaseFactory& useCases, QObject* parent)
    : QObject(parent), useCases_(useCases) {
    reload();
}

void BoardViewModel::reload() {
    QVariantList pages;
    QVariantList dock;
    try {
        const auto board = useCases_.getBoard().execute();
        for (const auto& page : board.pages()) {
            QVariantList pageApplications;
            for (const auto& application : page) {
                pageApplications.append(toVariant(application));
            }
            pages.append(QVariant(pageApplications));
        }
        for (const auto& application : board.dock()) {
            dock.append(toVariant(application));
        }
    } catch (const std::exception& exception) {
        qWarning() << "reboard: failed to build the board:" << exception.what();
    }
    pages_ = pages;
    dock_ = dock;
    emit boardChanged();
}

}  // namespace reboard::ui
