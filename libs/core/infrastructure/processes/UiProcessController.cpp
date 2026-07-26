#include "infrastructure/processes/UiProcessController.h"

#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "infrastructure/processes/UiDirective.h"

namespace reboard::infrastructure {

UiProcessController::UiProcessController(std::string uiBinaryPath)
    : uiBinaryPath_(std::move(uiBinaryPath)) {}

std::optional<std::string> UiProcessController::runUntilExit(
    const std::function<bool()>& shutdownRequested) {
    int pipeFds[2];
    if (::pipe(pipeFds) != 0) {
        throw std::runtime_error(std::string("pipe failed: ") + std::strerror(errno));
    }

    const pid_t pid = ::fork();
    if (pid < 0) {
        ::close(pipeFds[0]);
        ::close(pipeFds[1]);
        throw std::runtime_error(std::string("fork failed: ") + std::strerror(errno));
    }
    if (pid == 0) {
        // Child: stdout carries the directive back to the daemon; stderr is
        // inherited so UI warnings still reach the journal. Die with the
        // daemon so a stray UI never keeps the display locked.
        ::dup2(pipeFds[1], STDOUT_FILENO);
        ::close(pipeFds[0]);
        ::close(pipeFds[1]);
        ::prctl(PR_SET_PDEATHSIG, SIGTERM);
        ::execlp(uiBinaryPath_.c_str(), uiBinaryPath_.c_str(), static_cast<char*>(nullptr));
        ::_exit(127);
    }

    ::close(pipeFds[1]);
    std::string output;
    char buffer[512];
    bool terminationSent = false;
    while (true) {
        const ssize_t bytesRead = ::read(pipeFds[0], buffer, sizeof(buffer));
        if (bytesRead > 0) {
            output.append(buffer, static_cast<std::size_t>(bytesRead));
            continue;
        }
        if (bytesRead < 0 && errno == EINTR) {
            if (shutdownRequested() && !terminationSent) {
                ::kill(pid, SIGTERM);
                terminationSent = true;
            }
            continue;
        }
        break;  // EOF or unrecoverable error: the UI is gone.
    }
    ::close(pipeFds[0]);
    ::waitpid(pid, nullptr, 0);

    return parseLaunchDirective(output);
}

}  // namespace reboard::infrastructure
