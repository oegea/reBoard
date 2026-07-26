#include "SettingsViewModel.h"

#include <sys/statvfs.h>

#include <QDir>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QVariantMap>

#include "infrastructure/repositories/BuiltInApplicationRepository.h"
#include "infrastructure/repositories/CompositeApplicationRepository.h"
#include "infrastructure/repositories/FileApplicationRepository.h"

namespace reboard::settings {

namespace {

constexpr const char* kLanguageKey = "language";

}  // namespace

SettingsViewModel::SettingsViewModel(QObject* parent) : QObject(parent) {
    // Storage of the user data partition.
    struct statvfs stats {};
    if (::statvfs("/home", &stats) == 0) {
        const auto free = static_cast<qint64>(stats.f_bavail) * static_cast<qint64>(stats.f_frsize);
        const auto total =
            static_cast<qint64>(stats.f_blocks) * static_cast<qint64>(stats.f_frsize);
        storageFree_ = QLocale().formattedDataSize(free, 1);
        storageTotal_ = QLocale().formattedDataSize(total, 1);
    }

    // Installed applications, from the same sources the launcher uses.
    std::vector<std::string> directories = {"/etc/reboard/apps", "/opt/etc/reboard/apps"};
    const QString home = qEnvironmentVariable("HOME");
    if (!home.isEmpty()) {
        directories.push_back(home.toStdString() + "/.config/reboard/apps");
    }
    std::vector<std::string> removableDirectories;
    if (!home.isEmpty()) {
        removableDirectories.push_back(home.toStdString() + "/.config/reboard/apps-store");
    }
    infrastructure::FileApplicationRepository files(directories, removableDirectories);
    infrastructure::BuiltInApplicationRepository builtIns;
    infrastructure::CompositeApplicationRepository applications({&files, &builtIns});
    for (const auto& application : applications.findAll()) {
        QVariantMap map;
        map.insert("name", QString::fromStdString(application.name().value()));
        map.insert("appId", QString::fromStdString(application.id().value()));
        applications_.append(map);
    }
}

QString SettingsViewModel::version() const { return QStringLiteral(REBOARD_VERSION); }

QString SettingsViewModel::build() const { return QStringLiteral(REBOARD_BUILD); }

QString SettingsViewModel::license() const { return QStringLiteral("GPL-3.0-or-later"); }

QString SettingsViewModel::licenseText() const {
    QFile file(QStringLiteral(":/legal/LICENSE"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral("GPL-3.0 license text unavailable in this build.");
    }
    return QString::fromUtf8(file.readAll());
}

QString SettingsViewModel::language() const {
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
    return config.value(kLanguageKey, QStringLiteral("system")).toString();
}

void SettingsViewModel::setLanguage(const QString& language) {
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
    if (config.value(kLanguageKey) == language) {
        return;
    }
    config.setValue(kLanguageKey, language);
    config.sync();
    emit languageChanged();
}

}  // namespace reboard::settings
