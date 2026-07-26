#pragma once

namespace reboard::infrastructure {

// Detects whether a physical keyboard (e.g. the Type Folio) is currently
// attached, by scanning /dev/input/event* for a device that advertises the
// usual typing keys and no touch axes. Cheap enough to poll periodically.
class EvdevKeyboardDetector {
public:
    static bool keyboardPresent();
};

}  // namespace reboard::infrastructure
