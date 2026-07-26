#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace reboard::settings {

// Read model for the Settings screens. Presentation-side only: it surfaces
// facts (version, storage, installed apps) and persists the few user
// preferences (language) through the shared reBoard configuration.
class SettingsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString build READ build CONSTANT)
    Q_PROPERTY(QString license READ license CONSTANT)
    Q_PROPERTY(QString storageFree READ storageFree CONSTANT)
    Q_PROPERTY(QString storageTotal READ storageTotal CONSTANT)
    Q_PROPERTY(QVariantList applications READ applications CONSTANT)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit SettingsViewModel(QObject* parent = nullptr);

    QString version() const;
    QString build() const;
    QString license() const;
    QString storageFree() const { return storageFree_; }
    QString storageTotal() const { return storageTotal_; }
    QVariantList applications() const { return applications_; }

    QString language() const;
    void setLanguage(const QString& language);

signals:
    void languageChanged();

private:
    QString storageFree_;
    QString storageTotal_;
    QVariantList applications_;
};

}  // namespace reboard::settings
