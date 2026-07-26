#pragma once

#include "domain/repositories/ProcessRepository.h"
#include "domain/repositories/SessionRepository.h"

namespace reboard::application {

// Stops whatever application currently owns the screen so the launcher can
// take over again. Triggered by the "go home" gesture.
class CloseForegroundApplicationUseCase {
public:
    CloseForegroundApplicationUseCase(domain::ProcessRepository& processRepository,
                                      domain::SessionRepository& sessionRepository)
        : processRepository_(processRepository), sessionRepository_(sessionRepository) {}

    // Returns true when a foreground application was actually closed.
    bool execute() {
        const auto foreground = sessionRepository_.foreground();
        if (!foreground) {
            return false;
        }
        processRepository_.stop(foreground->handle());
        sessionRepository_.clear();
        return true;
    }

private:
    domain::ProcessRepository& processRepository_;
    domain::SessionRepository& sessionRepository_;
};

}  // namespace reboard::application
