#include "infrastructure/input/EvdevKeyboardDetector.h"

#include <string>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace reboard::infrastructure {

namespace {

constexpr int kMaxEventDevices = 32;

bool hasBit(const unsigned long* bits, unsigned int bit) {
    constexpr unsigned int kBitsPerLong = 8 * sizeof(unsigned long);
    return (bits[bit / kBitsPerLong] >> (bit % kBitsPerLong)) & 1UL;
}

}  // namespace

bool EvdevKeyboardDetector::keyboardPresent() {
    for (int index = 0; index < kMaxEventDevices; ++index) {
        const std::string path = "/dev/input/event" + std::to_string(index);
        const int fd = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }

        unsigned long keyBits[(KEY_MAX / (8 * sizeof(unsigned long))) + 1] = {};
        unsigned long absBits[(ABS_MAX / (8 * sizeof(unsigned long))) + 1] = {};
        const bool hasKeys =
            ::ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) >= 0;
        const bool hasAbs = ::ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) >= 0;
        ::close(fd);

        if (!hasKeys) {
            continue;
        }
        // A touch screen or pen digitizer also reports keys; exclude devices
        // with absolute axes. A real keyboard advertises letters and Enter
        // (unlike gpio buttons, which only expose power/home keys).
        if (hasAbs && (hasBit(absBits, ABS_X) || hasBit(absBits, ABS_MT_POSITION_X))) {
            continue;
        }
        if (hasBit(keyBits, KEY_Q) && hasBit(keyBits, KEY_SPACE) && hasBit(keyBits, KEY_ENTER)) {
            return true;
        }
    }
    return false;
}

}  // namespace reboard::infrastructure
