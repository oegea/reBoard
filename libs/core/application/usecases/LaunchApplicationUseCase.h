#pragma once

#include "domain/repositories/ApplicationRepository.h"
#include "domain/repositories/ProcessRepository.h"
#include "domain/repositories/SessionRepository.h"
#include "domain/valueobjects/ApplicationId.h"

namespace reboard::application {

enum class LaunchResult {
    Launched,
    NotFound,
};

// Launches an application by id. Any application currently in the foreground
// is stopped first, because on an e-paper device only one application can own
// the screen at a time.
class LaunchApplicationUseCase {
public:
    LaunchApplicationUseCase(const domain::ApplicationRepository& applicationRepository,
                             domain::ProcessRepository& processRepository,
                             domain::SessionRepository& sessionRepository)
        : applicationRepository_(applicationRepository),
          processRepository_(processRepository),
          sessionRepository_(sessionRepository) {}

    LaunchResult execute(const domain::ApplicationId& id) {
        const auto applications = applicationRepository_.findAll();
        const domain::Application* found = nullptr;
        for (const auto& application : applications) {
            if (application.id() == id) {
                found = &application;
                break;
            }
        }
        if (found == nullptr) {
            return LaunchResult::NotFound;
        }

        if (const auto foreground = sessionRepository_.foreground()) {
            processRepository_.stop(foreground->handle());
            sessionRepository_.clear();
        }

        const auto handle = processRepository_.launch(found->launchTarget());
        sessionRepository_.setForeground(domain::ForegroundApplication(found->id(), handle));
        return LaunchResult::Launched;
    }

private:
    const domain::ApplicationRepository& applicationRepository_;
    domain::ProcessRepository& processRepository_;
    domain::SessionRepository& sessionRepository_;
};

}  // namespace reboard::application
