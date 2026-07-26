#pragma once

#include "domain/repositories/ProcessRepository.h"
#include "domain/repositories/SessionRepository.h"

namespace reboard::application {

enum class ForegroundState {
    None,     // Nothing is tracked as foreground.
    Running,  // The tracked foreground application is still alive.
    Exited,   // The tracked foreground application died on its own.
};

// Reconciles the tracked foreground application with reality, so the launcher
// can show itself again when the foreground application exits by itself.
class RefreshForegroundStateUseCase {
public:
    RefreshForegroundStateUseCase(const domain::ProcessRepository& processRepository,
                                  domain::SessionRepository& sessionRepository)
        : processRepository_(processRepository), sessionRepository_(sessionRepository) {}

    ForegroundState execute() {
        const auto foreground = sessionRepository_.foreground();
        if (!foreground) {
            return ForegroundState::None;
        }
        if (processRepository_.isRunning(foreground->handle())) {
            return ForegroundState::Running;
        }
        sessionRepository_.clear();
        return ForegroundState::Exited;
    }

private:
    const domain::ProcessRepository& processRepository_;
    domain::SessionRepository& sessionRepository_;
};

}  // namespace reboard::application
