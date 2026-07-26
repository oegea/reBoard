#include "ui/TouchGestureMonitor.h"

#include <QDebug>

#include "infrastructure/input/EvdevTouchScreen.h"

namespace reboard::ui {

TouchGestureMonitor::TouchGestureMonitor(std::string devicePathOverride, bool invertX, bool invertY,
                                         QObject* parent)
    : QThread(parent),
      devicePathOverride_(std::move(devicePathOverride)),
      invertX_(invertX),
      invertY_(invertY) {}

TouchGestureMonitor::~TouchGestureMonitor() {
    requestStop();
    wait(2000);
}

void TouchGestureMonitor::requestStop() {
    stopRequested_.store(true);
}

void TouchGestureMonitor::run() {
    std::string devicePath = devicePathOverride_;
    if (devicePath.empty()) {
        const auto detected = infrastructure::EvdevTouchScreen::findTouchDevice();
        if (!detected) {
            qWarning() << "reboard: no touch device found; the home gesture is disabled";
            return;
        }
        devicePath = *detected;
    }

    try {
        infrastructure::EvdevTouchScreen touchScreen(devicePath, invertX_, invertY_);
        domain::GestureDetector detector;
        touchScreen.run(
            [this, &detector](const domain::TouchSample& sample) {
                if (detector.feed(sample)) {
                    emit homeGestureDetected();
                }
            },
            [this, &detector](std::uint64_t nowMs) {
                if (detector.poll(nowMs)) {
                    emit homeGestureDetected();
                }
            },
            stopRequested_);
    } catch (const std::exception& exception) {
        qWarning() << "reboard: touch monitoring stopped:" << exception.what();
    }
}

}  // namespace reboard::ui
