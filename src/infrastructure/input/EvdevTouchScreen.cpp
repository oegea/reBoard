#include "infrastructure/input/EvdevTouchScreen.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstddef>
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
constexpr long kTickIntervalUs = 100 * 1000;

std::uint64_t monotonicNowMs() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
}

bool hasBit(const unsigned long* bits, unsigned int bit) {
    constexpr unsigned int kBitsPerLong = 8 * sizeof(unsigned long);
    return (bits[bit / kBitsPerLong] >> (bit % kBitsPerLong)) & 1UL;
}

double normalize(int value, const input_absinfo& info, bool invert) {
    const int range = info.maximum - info.minimum;
    if (range <= 0) {
        return 0.0;
    }
    double normalized = static_cast<double>(value - info.minimum) / range;
    if (invert) {
        normalized = 1.0 - normalized;
    }
    return std::clamp(normalized, 0.0, 1.0);
}

class FileDescriptor {
public:
    explicit FileDescriptor(int fd) : fd_(fd) {}
    ~FileDescriptor() {
        if (fd_ >= 0) {
            ::close(fd_);
        }
    }
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    int get() const { return fd_; }

private:
    int fd_;
};

}  // namespace

EvdevTouchScreen::EvdevTouchScreen(std::string devicePath, bool invertX, bool invertY)
    : devicePath_(std::move(devicePath)), invertX_(invertX), invertY_(invertY) {}

std::optional<std::string> EvdevTouchScreen::findTouchDevice() {
    for (int index = 0; index < kMaxEventDevices; ++index) {
        const std::string path = "/dev/input/event" + std::to_string(index);
        const FileDescriptor fd(::open(path.c_str(), O_RDONLY | O_NONBLOCK));
        if (fd.get() < 0) {
            continue;
        }
        unsigned long absBits[(ABS_MAX / (8 * sizeof(unsigned long))) + 1] = {};
        if (::ioctl(fd.get(), EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) < 0) {
            continue;
        }
        if (hasBit(absBits, ABS_MT_POSITION_X) && hasBit(absBits, ABS_MT_POSITION_Y)) {
            return path;
        }
    }
    return std::nullopt;
}

void EvdevTouchScreen::run(const SampleCallback& onSample, const TickCallback& onTick,
                           const std::atomic<bool>& stopRequested) {
    const FileDescriptor fd(::open(devicePath_.c_str(), O_RDONLY | O_NONBLOCK));
    if (fd.get() < 0) {
        throw std::runtime_error("Cannot open touch device " + devicePath_ + ": " +
                                 std::strerror(errno));
    }

    input_absinfo absX{};
    input_absinfo absY{};
    if (::ioctl(fd.get(), EVIOCGABS(ABS_MT_POSITION_X), &absX) < 0 ||
        ::ioctl(fd.get(), EVIOCGABS(ABS_MT_POSITION_Y), &absY) < 0) {
        throw std::runtime_error("Cannot query touch device ranges for " + devicePath_);
    }

    int currentSlot = 0;
    bool touching = false;
    bool pendingDown = false;
    bool pendingUp = false;
    bool positionChanged = false;
    int rawX = 0;
    int rawY = 0;

    while (!stopRequested.load()) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd.get(), &readSet);
        timeval timeout{0, kTickIntervalUs};

        const int ready = ::select(fd.get() + 1, &readSet, nullptr, nullptr, &timeout);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::runtime_error(std::string("select failed on touch device: ") +
                                     std::strerror(errno));
        }
        if (ready == 0) {
            onTick(monotonicNowMs());
            continue;
        }

        input_event events[64];
        const ssize_t bytesRead = ::read(fd.get(), events, sizeof(events));
        if (bytesRead <= 0) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            throw std::runtime_error(std::string("read failed on touch device: ") +
                                     std::strerror(errno));
        }

        const auto eventCount = static_cast<std::size_t>(bytesRead) / sizeof(input_event);
        for (std::size_t i = 0; i < eventCount; ++i) {
            const input_event& event = events[i];
            if (event.type == EV_ABS) {
                if (event.code == ABS_MT_SLOT) {
                    currentSlot = event.value;
                    continue;
                }
                if (currentSlot != 0) {
                    continue;  // Track the first finger only.
                }
                switch (event.code) {
                    case ABS_MT_TRACKING_ID:
                        if (event.value >= 0) {
                            pendingDown = true;
                        } else {
                            pendingUp = true;
                        }
                        break;
                    case ABS_MT_POSITION_X:
                        rawX = event.value;
                        positionChanged = true;
                        break;
                    case ABS_MT_POSITION_Y:
                        rawY = event.value;
                        positionChanged = true;
                        break;
                    default:
                        break;
                }
                continue;
            }

            if (event.type != EV_SYN || event.code != SYN_REPORT) {
                continue;
            }

            // End of packet: translate the accumulated state into a sample.
            const std::uint64_t nowMs = monotonicNowMs();
            const double x = normalize(rawX, absX, invertX_);
            const double y = normalize(rawY, absY, invertY_);

            if (pendingDown && !touching) {
                touching = true;
                onSample(domain::TouchSample(x, y, nowMs, domain::TouchPhase::Down));
            } else if (pendingUp && touching) {
                touching = false;
                onSample(domain::TouchSample(x, y, nowMs, domain::TouchPhase::Up));
            } else if (positionChanged && touching) {
                onSample(domain::TouchSample(x, y, nowMs, domain::TouchPhase::Move));
            }
            pendingDown = false;
            pendingUp = false;
            positionChanged = false;
        }
    }
}

}  // namespace reboard::infrastructure
