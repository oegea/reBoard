#include "LocaleResolver.h"

#include <QSettings>
#include <QString>

namespace reboard::rekit {

QLocale resolveLocale() {
    const QString envOverride = qEnvironmentVariable("REBOARD_LOCALE");
    if (!envOverride.isEmpty()) {
        return QLocale(envOverride);
    }

    QSettings config(QSettings::IniFormat, QSettings::UserScope, "reboard", "reboard");
    const QString language = config.value("language", "system").toString();
    if (language != QStringLiteral("system") && !language.isEmpty()) {
        return QLocale(language);
    }

    // "System" on a reMarkable means the language chosen in the stock UI.
    const QString home = qEnvironmentVariable("HOME");
    if (!home.isEmpty()) {
        QSettings xochitl(home + "/.config/remarkable/xochitl.conf", QSettings::IniFormat);
        const QString stockLanguage = xochitl.value("Language").toString();
        if (!stockLanguage.isEmpty()) {
            return QLocale(stockLanguage);
        }
    }

    return QLocale::system();
}

}  // namespace reboard::rekit
