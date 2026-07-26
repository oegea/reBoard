#pragma once

#include <atomic>
#include <string>

#include <QThread>

namespace reboard::ui {

// Watches the hardware power button while the board is on screen so the
// launcher can show the lock screen (story 007, phase 1).
class PowerButtonMonitor : public QThread {
    Q_OBJECT

public:
    // `devicePathOverride` may be empty to auto-detect.
    explicit PowerButtonMonitor(std::string devicePathOverride, QObject* parent = nullptr);
    ~PowerButtonMonitor() override;

    void requestStop();

signals:
    void powerButtonPressed();

protected:
    void run() override;

private:
    std::string devicePathOverride_;
    std::atomic<bool> stopRequested_{false};
};

}  // namespace reboard::ui
