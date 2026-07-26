#include "StoreViewModel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>

#include "infrastructure/system/DeviceSlug.h"

namespace reboard::store {

namespace {
constexpr const char* kDefaultRepository =
    "https://raw.githubusercontent.com/oegea/reBoard/main/store";
}

namespace {

// Errors that mean "no usable connection" rather than a broken catalog.
bool isConnectivityError(QNetworkReply::NetworkError error) {
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
        case QNetworkReply::HostNotFoundError:
        case QNetworkReply::TimeoutError:
        case QNetworkReply::OperationCanceledError:  // Transfer timeout.
        case QNetworkReply::TemporaryNetworkFailureError:
        case QNetworkReply::NetworkSessionFailedError:
        case QNetworkReply::UnknownNetworkError:
            return true;
        default:
            return false;
    }
}

}  // namespace

StoreViewModel::StoreViewModel(application::UseCaseFactory& useCases, QObject* parent)
    : QObject(parent),
      useCases_(useCases),
      deviceSlug_(QString::fromStdString(infrastructure::currentDeviceSlug())) {
    // Never hang on "Loading": a sleepy Wi-Fi radio must surface as an
    // error state the user can act on.
    network_.setTransferTimeout(15000);
    reload();
}

void StoreViewModel::quitStore() { QCoreApplication::quit(); }

QString StoreViewModel::repositoryUrl() const {
    const QString envOverride = qEnvironmentVariable("REBOARD_STORE_URL");
    if (!envOverride.isEmpty()) {
        return envOverride;
    }
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
    return config.value("store/repository", QString::fromLatin1(kDefaultRepository)).toString();
}

void StoreViewModel::reload() {
    catalogAttempt_ = 0;
    fetchCatalog();
}

void StoreViewModel::fetchCatalog() {
    static constexpr int kMaxAttempts = 3;
    ++catalogAttempt_;
    loading_ = true;
    retrying_ = catalogAttempt_ > 1;
    offline_ = false;
    errorMessage_.clear();
    emit stateChanged();

    QNetworkReply* reply =
        network_.get(QNetworkRequest(QUrl(repositoryUrl() + "/catalog.json")));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "reboard-store: catalog fetch attempt" << catalogAttempt_
                       << "failed:" << reply->error() << reply->errorString();
            // The Wi-Fi radio sleeps aggressively on e-paper devices and the
            // first request often just wakes it up: retry quietly before
            // bothering the user.
            if (isConnectivityError(reply->error()) && catalogAttempt_ < kMaxAttempts) {
                QTimer::singleShot(catalogAttempt_ == 1 ? 2000 : 4000, this,
                                   [this] { fetchCatalog(); });
                return;
            }
            loading_ = false;
            retrying_ = false;
            sections_.clear();
            catalogPaths_ = QVariantMap();
            offline_ = isConnectivityError(reply->error());
            errorMessage_ = reply->errorString();
            emit stateChanged();
            return;
        }

        loading_ = false;
        retrying_ = false;
        sections_.clear();
        catalogPaths_ = QVariantMap();
        const QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isObject()) {
            errorMessage_ = tr("The catalog is not valid.");
            emit stateChanged();
            return;
        }

        // Group applications per category, honoring the catalog order.
        QStringList categories;
        for (const auto& value : document.object().value("categories").toArray()) {
            categories.append(value.toString());
        }
        QMap<QString, QVariantList> grouped;
        for (const auto& value : document.object().value("apps").toArray()) {
            const QJsonObject app = value.toObject();
            const QString id = app.value("id").toString();
            const QString category = app.value("category").toString();
            if (id.isEmpty()) {
                continue;
            }
            catalogPaths_.insert(id, app.value("path").toString());
            QVariantMap row;
            row.insert("appId", id);
            row.insert("name", app.value("name").toString());
            row.insert("summary", app.value("summary").toString());
            grouped[category].append(row);
            if (!categories.contains(category)) {
                categories.append(category);
            }
        }
        for (const QString& category : categories) {
            if (!grouped.contains(category)) {
                continue;
            }
            QVariantMap section;
            section.insert("category", category);
            section.insert("apps", grouped.value(category));
            sections_.append(section);
        }
        emit stateChanged();
    });
}

void StoreViewModel::openApp(const QString& appId) {
    const QString path = catalogPaths_.value(appId).toString();
    if (path.isEmpty()) {
        return;
    }
    detail_ = QVariantMap();
    detail_.insert("appId", appId);
    detail_.insert("loading", true);
    detailVisible_ = true;
    emit detailChanged();

    QNetworkReply* reply =
        network_.get(QNetworkRequest(QUrl(repositoryUrl() + "/" + path + "/app.json")));
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId] {
        reply->deleteLater();
        if (!detailVisible_ || detail_.value("appId").toString() != appId) {
            return;  // The user already left this page.
        }
        if (reply->error() != QNetworkReply::NoError) {
            setDetailField("loading", false);
            setDetailField("error", reply->errorString());
            return;
        }
        const QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isObject()) {
            setDetailField("loading", false);
            setDetailField("error", tr("The application record is not valid."));
            return;
        }
        finishDetail(document.object().toVariantMap());
    });
}

void StoreViewModel::finishDetail(const QVariantMap& record) {
    const QString appId = detail_.value("appId").toString();
    const QVariantMap downloads = record.value("downloads").toMap();
    const QString downloadUrl = downloads.value(deviceSlug_).toString();

    detail_.insert("loading", false);
    detail_.insert("name", record.value("name"));
    detail_.insert("category", record.value("category"));
    detail_.insert("version", record.value("version"));
    detail_.insert("description", record.value("description"));
    detail_.insert("available", !downloadUrl.isEmpty());
    detail_.insert("downloadUrl", downloadUrl);
    try {
        const domain::ApplicationId id(appId.toStdString());
        const bool installed = useCases_.checkInstalled().execute(id);
        detail_.insert("installed", installed);
        if (installed) {
            const auto installedVersion = useCases_.checkInstalled().installedVersion(id);
            const QString current =
                installedVersion ? QString::fromStdString(*installedVersion) : QString();
            detail_.insert("installedVersion", current);
            // An empty recorded version means a pre-versioning install:
            // offer the update too.
            detail_.insert("updateAvailable",
                           current != record.value("version").toString());
        }
    } catch (const std::exception&) {
        detail_.insert("installed", false);
    }
    emit detailChanged();
}

void StoreViewModel::closeDetail() {
    detailVisible_ = false;
    detail_ = QVariantMap();
    busy_ = false;
    emit detailChanged();
}

void StoreViewModel::install() {
    const QString appId = detail_.value("appId").toString();
    const QUrl downloadUrl(detail_.value("downloadUrl").toString());
    if (appId.isEmpty() || !downloadUrl.isValid() || busy_) {
        return;
    }
    busy_ = true;
    setDetailField("error", QString());

    QNetworkReply* reply = network_.get(QNetworkRequest(downloadUrl));
    connect(reply, &QNetworkReply::finished, this, [this, reply, appId] {
        reply->deleteLater();
        busy_ = false;
        if (!detailVisible_ || detail_.value("appId").toString() != appId) {
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            setDetailField("error", reply->errorString());
            return;
        }
        const QString packagePath =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/reboard-" +
            appId + ".tar.gz";
        QFile packageFile(packagePath);
        if (!packageFile.open(QIODevice::WriteOnly)) {
            setDetailField("error", tr("Cannot write the downloaded package."));
            return;
        }
        packageFile.write(reply->readAll());
        packageFile.close();

        try {
            useCases_.installPackage().execute(domain::ApplicationId(appId.toStdString()),
                                               packagePath.toStdString(),
                                               detail_.value("version").toString().toStdString());
            setDetailField("installedVersion", detail_.value("version"));
            setDetailField("updateAvailable", false);
            setDetailField("installed", true);
        } catch (const std::exception& exception) {
            setDetailField("error", QString::fromUtf8(exception.what()));
        }
        QFile::remove(packagePath);
    });
    emit detailChanged();
}

void StoreViewModel::removeInstalled() {
    const QString appId = detail_.value("appId").toString();
    if (appId.isEmpty()) {
        return;
    }
    try {
        useCases_.uninstallApplication().execute(domain::ApplicationId(appId.toStdString()));
        setDetailField("installed", false);
    } catch (const std::exception& exception) {
        setDetailField("error", QString::fromUtf8(exception.what()));
    }
}

void StoreViewModel::setDetailField(const QString& key, const QVariant& value) {
    detail_.insert(key, value);
    emit detailChanged();
}

}  // namespace reboard::store
