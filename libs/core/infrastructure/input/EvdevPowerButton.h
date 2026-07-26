#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>

namespace reboard::infrastructure {

// Watches the hardware power button (a dedicated evdev device; on the
// reMarkable 2 it is the snvs-powerkey). systemd-logind is configured with
// HandlePowerKey=ignore on the device, so reBoard is free to own the
// button's behaviour.
class EvdevPowerButton {
public:
    using PressCallback = std::function<void()>;

    explicit EvdevPowerButton(std::string devicePath);

    // First device advertising KEY_POWER that is not a full keyboard.
    static std::optional<std::string> findPowerButtonDevice();

    // Blocking loop: invokes the callback on every press (key-down) until
    // `stopRequested` becomes true.
    void run(const PressCallback& onPress, const std::atomic<bool>& stopRequested);

private:
    std::string devicePath_;
};

}  // namespace reboard::infrastructure
