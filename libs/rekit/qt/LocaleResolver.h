#pragma once

#include <QLocale>

namespace reboard::rekit {

// Single locale policy for every reBoard app (ADR-0004):
//   1. REBOARD_LOCALE environment variable (testing/override);
//   2. the user's choice in reBoard Settings (shared reboard.ini);
//   3. the language configured in the stock UI (xochitl.conf), so "system"
//      matches what the user already picked on the device;
//   4. QLocale::system().
QLocale resolveLocale();

}  // namespace reboard::rekit
