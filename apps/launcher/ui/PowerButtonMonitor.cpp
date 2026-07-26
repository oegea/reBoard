#include "PowerButtonMonitor.h"

#include <QDebug>

#include "infrastructure/input/EvdevPowerButton.h"

namespace reboard::ui {

PowerButtonMonitor::PowerButtonMonitor(std::string devicePathOverride, QObject* parent)
    : QThread(parent), devicePathOverride_(std::move(devicePathOverride)) {}

PowerButtonMonitor::~PowerButtonMonitor() {
    requestStop();
    wait(2000);
}

void PowerButtonMonitor::requestStop() {
    stopRequested_.store(true);
}

void PowerButtonMonitor::run() {
    std::string devicePath = devicePathOverride_;
    if (devicePath.empty()) {
        const auto detected = infrastructure::EvdevPowerButton::findPowerButtonDevice();
        if (!detected) {
            qWarning() << "reboard-ui: no power button device found; lock screen disabled";
            return;
        }
        devicePath = *detected;
    }

    try {
        infrastructure::EvdevPowerButton powerButton(devicePath);
        powerButton.run([this] { emit powerButtonPressed(); }, stopRequested_);
    } catch (const std::exception& exception) {
        qWarning() << "reboard-ui: power button monitoring stopped:" << exception.what();
    }
}

}  // namespace reboard::ui
