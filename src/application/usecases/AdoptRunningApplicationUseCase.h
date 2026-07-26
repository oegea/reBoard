#pragma once

#include "domain/repositories/ApplicationRepository.h"
#include "domain/repositories/ProcessRepository.h"
#include "domain/repositories/SessionRepository.h"

namespace reboard::application {

// When reBoard starts, another application (typically xochitl) may already be
// on screen. This use case detects running unit-based applications and adopts
// them as the current foreground so the home gesture can close them.
class AdoptRunningApplicationUseCase {
public:
    AdoptRunningApplicationUseCase(const domain::ApplicationRepository& applicationRepository,
                                   const domain::ProcessRepository& processRepository,
                                   domain::SessionRepository& sessionRepository)
        : applicationRepository_(applicationRepository),
          processRepository_(processRepository),
          sessionRepository_(sessionRepository) {}

    // Returns true when a running application was adopted.
    bool execute() {
        if (sessionRepository_.foreground()) {
            return false;
        }
        for (const auto& application : applicationRepository_.findAll()) {
            if (application.launchTarget().type() != domain::LaunchType::SystemdUnit) {
                continue;
            }
            const auto handle = domain::ProcessHandle::forUnit(application.launchTarget().unitName());
            if (processRepository_.isRunning(handle)) {
                sessionRepository_.setForeground(domain::ForegroundApplication(application.id(), handle));
                return true;
            }
        }
        return false;
    }

private:
    const domain::ApplicationRepository& applicationRepository_;
    const domain::ProcessRepository& processRepository_;
    domain::SessionRepository& sessionRepository_;
};

}  // namespace reboard::application
