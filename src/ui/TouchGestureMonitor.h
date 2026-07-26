#pragma once

#include <atomic>
#include <string>

#include <QThread>

#include "domain/services/GestureDetector.h"

namespace reboard::ui {

// Resident background thread that keeps reading the touch screen even while
// a third-party application owns the display, so the "go home" gesture always
// works. Runs the pure domain GestureDetector over evdev input.
class TouchGestureMonitor : public QThread {
    Q_OBJECT

public:
    // `devicePathOverride` may be empty to auto-detect the touch device.
    TouchGestureMonitor(std::string devicePathOverride, bool invertX, bool invertY,
                        QObject* parent = nullptr);
    ~TouchGestureMonitor() override;

    void requestStop();

signals:
    void homeGestureDetected();

protected:
    void run() override;

private:
    std::string devicePathOverride_;
    bool invertX_;
    bool invertY_;
    std::atomic<bool> stopRequested_{false};
};

}  // namespace reboard::ui
