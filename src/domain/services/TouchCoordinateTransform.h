#pragma once

#include "domain/valueobjects/TouchSample.h"

namespace reboard::domain {

// Maps a physical (screen-space) touch sample into the logical coordinate
// system of UI content rotated by `rotationDegrees` (0, 90 or 270, clockwise).
// This keeps the "swipe up from the bottom edge" gesture anchored to the
// VISUAL bottom in every orientation, including while another application
// owns the display.
//
// For content rotated 90° clockwise the visual bottom lies on the physical
// left edge; for 270° it lies on the physical right edge.
inline TouchSample rotateTouchSample(const TouchSample& sample, int rotationDegrees) {
    switch (rotationDegrees) {
        case 90:
            // Inverse of a 90° clockwise content rotation.
            return TouchSample(sample.y(), 1.0 - sample.x(), sample.timestampMs(), sample.phase());
        case 270:
            return TouchSample(1.0 - sample.y(), sample.x(), sample.timestampMs(), sample.phase());
        default:
            return sample;
    }
}

}  // namespace reboard::domain
