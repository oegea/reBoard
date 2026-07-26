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

    // Politely stops whatever the handle points at.
    virtual void stop(const ProcessHandle& handle) = 0;

    virtual bool isRunning(const ProcessHandle& handle) const = 0;
};

}  // namespace reboard::domain
