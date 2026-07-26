#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include "application/UseCaseFactory.h"

namespace reboard::store {

// Drives the App Store screens: fetches the catalog and app records from
// the configured repository, downloads device packages and delegates
// install/uninstall to the core use cases (ADR-0006).
class StoreViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY stateChanged)
    Q_PROPERTY(bool retrying READ retrying NOTIFY stateChanged)
    Q_PROPERTY(bool offline READ offline NOTIFY stateChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY stateChanged)
    Q_PROPERTY(QVariantList sections READ sections NOTIFY stateChanged)
    Q_PROPERTY(bool detailVisible READ detailVisible NOTIFY detailChanged)
    Q_PROPERTY(QVariantMap detail READ detail NOTIFY detailChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY detailChanged)
    Q_PROPERTY(QString repositoryUrl READ repositoryUrl CONSTANT)

public:
    explicit StoreViewModel(application::UseCaseFactory& useCases, QObject* parent = nullptr);

    bool loading() const { return loading_; }
    bool retrying() const { return retrying_; }
    bool offline() const { return offline_; }
    QString errorMessage() const { return errorMessage_; }
    QVariantList sections() const { return sections_; }
    bool detailVisible() const { return detailVisible_; }
    QVariantMap detail() const { return detail_; }
    bool busy() const { return busy_; }
    QString repositoryUrl() const;

    Q_INVOKABLE void reload();
    // Leaves the store (the daemon shows the board again); used by the
    // offline message.
    Q_INVOKABLE void quitStore();
    Q_INVOKABLE void openApp(const QString& appId);
    Q_INVOKABLE void closeDetail();
    Q_INVOKABLE void install();
    Q_INVOKABLE void removeInstalled();

signals:
    void stateChanged();
    void detailChanged();

private:
    void fetchCatalog();
    void setDetailField(const QString& key, const QVariant& value);
    void finishDetail(const QVariantMap& record);

    application::UseCaseFactory& useCases_;
    QNetworkAccessManager network_;
    QString deviceSlug_;

    bool loading_ = false;
    bool retrying_ = false;
    bool offline_ = false;
    int catalogAttempt_ = 0;
    QString errorMessage_;
    QVariantList sections_;
    QVariantMap catalogPaths_;  // appId -> folder path within the repository.

    bool detailVisible_ = false;
    bool busy_ = false;
    QVariantMap detail_;
};

}  // namespace reboard::store
