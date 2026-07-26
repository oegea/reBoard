#include "BoardViewModel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVariantMap>

namespace reboard::ui {

namespace {

// Built-in names live in the (translation-free) core; localize them at the
// presentation layer, but only when the user has not overridden the entry.
QString displayName(const domain::Application& application) {
    const QString name = QString::fromStdString(application.name().value());
    const std::string& id = application.id().value();
    if (id == "xochitl" && name == QStringLiteral("Notebooks")) {
        return QCoreApplication::translate("BuiltInApps", "Notebooks");
    }
    if (id == "settings" && name == QStringLiteral("Settings")) {
        return QCoreApplication::translate("BuiltInApps", "Settings");
    }
    if (id == "store" && name == QStringLiteral("App Store")) {
        return QCoreApplication::translate("BuiltInApps", "App Store");
    }
    return name;
}

QVariantMap toVariant(const domain::Application& application) {
    QVariantMap map;
    map.insert("appId", QString::fromStdString(application.id().value()));
    map.insert("name", displayName(application));
    map.insert("icon", QString::fromStdString(application.iconPath()));
    map.insert("showReturnHint", application.showReturnHint());
    map.insert("removable", application.removable());
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
