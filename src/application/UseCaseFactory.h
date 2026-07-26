#pragma once

#include <string>
#include <vector>

#include "application/usecases/AdoptRunningApplicationUseCase.h"
#include "application/usecases/CloseForegroundApplicationUseCase.h"
#include "application/usecases/GetBoardUseCase.h"
#include "application/usecases/LaunchApplicationUseCase.h"
#include "application/usecases/RefreshForegroundStateUseCase.h"
#include "infrastructure/processes/PosixProcessRepository.h"
#include "infrastructure/repositories/BuiltInApplicationRepository.h"
#include "infrastructure/repositories/CompositeApplicationRepository.h"
#include "infrastructure/repositories/FileApplicationRepository.h"
#include "infrastructure/repositories/InMemorySessionRepository.h"

namespace reboard::application {

// Composition root of the business layer: wires the use cases with the
// infrastructure implementations of the domain repositories. This is the only
// place in the application layer allowed to depend on infrastructure.
class UseCaseFactory {
public:
    explicit UseCaseFactory(std::vector<std::string> manifestDirectories)
        : fileApplications_(std::move(manifestDirectories)),
          applications_({&fileApplications_, &builtInApplications_}),
          getBoard_(applications_),
          launchApplication_(applications_, processes_, session_),
          closeForegroundApplication_(processes_, session_),
          adoptRunningApplication_(applications_, processes_, session_),
          refreshForegroundState_(processes_, session_) {}

    GetBoardUseCase& getBoard() { return getBoard_; }
    LaunchApplicationUseCase& launchApplication() { return launchApplication_; }
    CloseForegroundApplicationUseCase& closeForegroundApplication() {
        return closeForegroundApplication_;
    }
    AdoptRunningApplicationUseCase& adoptRunningApplication() { return adoptRunningApplication_; }
    RefreshForegroundStateUseCase& refreshForegroundState() { return refreshForegroundState_; }

private:
    infrastructure::FileApplicationRepository fileApplications_;
    infrastructure::BuiltInApplicationRepository builtInApplications_;
    infrastructure::CompositeApplicationRepository applications_;
    infrastructure::PosixProcessRepository processes_;
    infrastructure::InMemorySessionRepository session_;

    GetBoardUseCase getBoard_;
    LaunchApplicationUseCase launchApplication_;
    CloseForegroundApplicationUseCase closeForegroundApplication_;
    AdoptRunningApplicationUseCase adoptRunningApplication_;
    RefreshForegroundStateUseCase refreshForegroundState_;
};

}  // namespace reboard::application
