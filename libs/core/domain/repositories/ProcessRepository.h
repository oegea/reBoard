#pragma once

#include "domain/valueobjects/LaunchTarget.h"
#include "domain/valueobjects/ProcessHandle.h"

namespace reboard::domain {

// Starts, stops and monitors external applications. Implemented by the
// infrastructure layer (POSIX processes and systemctl).
class ProcessRepository {
public:
    virtual ~ProcessRepository() = default;

    // Starts the target and returns a handle to control it. Throws
    // std::runtime_error when the target cannot be started.
    virtual ProcessHandle launch(const LaunchTarget& target) = 0;

    // Stops whatever the handle points at and does not return until it has
    // actually terminated (escalating if needed). This guarantee matters on
    // e-paper devices: the display can only be owned by one process at a
    // time, so the next application must never start while the previous one
    // is still alive.
    virtual void stop(const ProcessHandle& handle) = 0;

    virtual bool isRunning(const ProcessHandle& handle) const = 0;

    // Whether the process behind the handle ended abnormally (crash or
    // non-zero exit). Only meaningful after it stopped running; systemd
    // units report false (the stock UI manages its own failures).
    virtual bool lastExitWasAbnormal(const ProcessHandle& handle) const {
        (void)handle;
        return false;
    }
};

}  // namespace reboard::domain
