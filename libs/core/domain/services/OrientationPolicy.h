#pragma once

#include <optional>
#include <string>

#include "domain/valueobjects/Orientation.h"

namespace reboard::domain {

// Decides how the launcher must be oriented. The reMarkable 2 has no
// accelerometer, so orientation derives from usage signals: a physical
// keyboard means the tablet sits in typing position (landscape); a manual
// override always wins. See docs/stories/001-orientation-detection.md.
class OrientationPolicy {
public:
    // `manualOverride` accepts "portrait" or "landscape" (anything else is
    // ignored); it typically comes from the REBOARD_ORIENTATION variable.
    static Orientation decide(bool keyboardPresent,
                              const std::optional<std::string>& manualOverride = std::nullopt) {
        if (manualOverride) {
            if (*manualOverride == "portrait") {
                return Orientation::Portrait;
            }
            if (*manualOverride == "landscape") {
                return Orientation::Landscape;
            }
        }
        return keyboardPresent ? Orientation::Landscape : Orientation::Portrait;
    }

    // Content rotation in degrees for an orientation. Landscape defaults to
    // 90° clockwise (visual bottom on the physical left edge, where the
    // Type Folio holds the device); 270 is the valid alternative.
    static int rotationDegrees(Orientation orientation, int landscapeRotation = 90) {
        if (orientation == Orientation::Portrait) {
            return 0;
        }
        return landscapeRotation == 270 ? 270 : 90;
    }
};

}  // namespace reboard::domain
