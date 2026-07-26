#include "infrastructure/input/EvdevPowerButton.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>

namespace reboard::infrastructure {

namespace {

constexpr int kMaxEventDevices = 32;
constexpr long kWakeIntervalUs = 200 * 1000;

bool hasBit(const unsigned long* bits, unsigned int bit) {
    constexpr unsigned int kBitsPerLong = 8 * sizeof(unsigned long);
    return (bits[bit / kBitsPerLong] >> (bit % kBitsPerLong)) & 1UL;
}

}  // namespace

EvdevPowerButton::EvdevPowerButton(std::string devicePath)
    : devicePath_(std::move(devicePath)) {}

std::optional<std::string> EvdevPowerButton::findPowerButtonDevice() {
    for (int index = 0; index < kMaxEventDevices; ++index) {
        const std::string path = "/dev/input/event" + std::to_string(index);
        const int fd = ::open(path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }
        unsigned long keyBits[(KEY_MAX / (8 * sizeof(unsigned long))) + 1] = {};
        const bool hasKeys = ::ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) >= 0;
        ::close(fd);
        // A dedicated power button advertises KEY_POWER but not letters
        // (which would make it a keyboard, e.g. the Type Folio).
        if (hasKeys && hasBit(keyBits, KEY_POWER) && !hasBit(keyBits, KEY_Q)) {
            return path;
        }
    }
    return std::nullopt;
}

void EvdevPowerButton::run(const PressCallback& onPress,
                           const std::atomic<bool>& stopRequested) {
    const int fd = ::open(devicePath_.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        throw std::runtime_error("Cannot open power button device " + devicePath_ + ": " +
                                 std::strerror(errno));
    }

    while (!stopRequested.load()) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd, &readSet);
        timeval timeout{0, kWakeIntervalUs};

        const int ready = ::select(fd + 1, &readSet, nullptr, nullptr, &timeout);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            ::close(fd);
            throw std::runtime_error(std::string("select failed on power button: ") +
                                     std::strerror(errno));
        }
        if (ready == 0) {
            continue;
        }

        input_event events[16];
        const ssize_t bytesRead = ::read(fd, events, sizeof(events));
        if (bytesRead <= 0) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            ::close(fd);
            throw std::runtime_error(std::string("read failed on power button: ") +
                                     std::strerror(errno));
        }

        const auto count = static_cast<std::size_t>(bytesRead) / sizeof(input_event);
        for (std::size_t i = 0; i < count; ++i) {
            if (events[i].type == EV_KEY && events[i].code == KEY_POWER &&
                events[i].value == 1) {
                onPress();
            }
        }
    }
    ::close(fd);
}

}  // namespace reboard::infrastructure
