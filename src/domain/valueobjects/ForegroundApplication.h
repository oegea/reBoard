#pragma once

#include "domain/valueobjects/ApplicationId.h"
#include "domain/valueobjects/ProcessHandle.h"

namespace reboard::domain {

// Immutable association between an application and the process handle that is
// currently occupying the screen.
class ForegroundApplication {
public:
    ForegroundApplication(ApplicationId id, ProcessHandle handle)
        : id_(std::move(id)), handle_(std::move(handle)) {}

    const ApplicationId& id() const noexcept { return id_; }
    const ProcessHandle& handle() const noexcept { return handle_; }

private:
    ApplicationId id_;
    ProcessHandle handle_;
};

}  // namespace reboard::domain
