// reBoard resident daemon: owns the home gesture and the application
// lifecycle. The Qt launcher UI runs as a separate short-lived process
// (reboard-ui) because the e-paper display can only be locked by one process
// at a time — the UI must be completely dead before another application
// (e.g. xochitl) starts, or the OS failure policy reboots the device.

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <limits.h>
#include <unistd.h>

#include "application/UseCaseFactory.h"
#include "domain/services/GestureDetector.h"
#include "infrastructure/input/EvdevTouchScreen.h"
#include "infrastructure/processes/UiProcessController.h"

namespace {

volatile std::sig_atomic_t gShutdownRequested = 0;

void handleTerminationSignal(int) { gShutdownRequested = 1; }

void installSignalHandlers() {
    struct sigaction action {};
    action.sa_handler = handleTerminationSignal;
    // No SA_RESTART: blocking reads must wake up so the daemon can exit.
    ::sigaction(SIGTERM, &action, nullptr);
    ::sigaction(SIGINT, &action, nullptr);
}

std::vector<std::string> manifestDirectories() {
    std::vector<std::string> directories = {"/etc/reboard/apps", "/opt/etc/reboard/apps"};
    if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
        directories.push_back(std::string(home) + "/.config/reboard/apps");
    }
    return directories;
}

// The UI binary lives next to the daemon unless overridden.
std::string uiBinaryPath() {
    if (const char* override = std::getenv("REBOARD_UI_BIN"); override != nullptr) {
        return override;
    }
    char selfPath[PATH_MAX] = {};
    const ssize_t length = ::readlink("/proc/self/exe", selfPath, sizeof(selfPath) - 1);
    if (length > 0) {
        std::string path(selfPath, static_cast<std::size_t>(length));
        const auto slash = path.find_last_of('/');
        if (slash != std::string::npos) {
            return path.substr(0, slash + 1) + "reboard-ui";
        }
    }
    return "reboard-ui";
}

}  // namespace

int main() {
    installSignalHandlers();

    reboard::application::UseCaseFactory useCases(manifestDirectories());
    try {
        useCases.adoptRunningApplication().execute();
    } catch (const std::exception& exception) {
        std::cerr << "reboard: could not adopt the running application: " << exception.what()
                  << std::endl;
    }

    // Resident touch monitoring: works even while another application owns
    // the display, which is the whole point of the home gesture.
    std::atomic<bool> stopTouchThread{false};
    std::atomic<bool> homeRequested{false};
    std::thread touchThread([&stopTouchThread, &homeRequested] {
        try {
            std::string devicePath;
            if (const char* override = std::getenv("REBOARD_TOUCH_DEVICE"); override != nullptr) {
                devicePath = override;
            } else if (const auto detected =
                           reboard::infrastructure::EvdevTouchScreen::findTouchDevice()) {
                devicePath = *detected;
            } else {
                std::cerr << "reboard: no touch device found; the home gesture is disabled"
                          << std::endl;
                return;
            }
            const bool invertX = std::getenv("REBOARD_TOUCH_INVERT_X") != nullptr;
            const bool invertY = std::getenv("REBOARD_TOUCH_NO_INVERT_Y") == nullptr;
            reboard::infrastructure::EvdevTouchScreen touchScreen(devicePath, invertX, invertY);
            reboard::domain::GestureDetector detector;
            touchScreen.run(
                [&detector, &homeRequested](const reboard::domain::TouchSample& sample) {
                    if (detector.feed(sample)) {
                        homeRequested.store(true);
                    }
                },
                [&detector, &homeRequested](std::uint64_t nowMs) {
                    if (detector.poll(nowMs)) {
                        homeRequested.store(true);
                    }
                },
                stopTouchThread);
        } catch (const std::exception& exception) {
            std::cerr << "reboard: touch monitoring stopped: " << exception.what() << std::endl;
        }
    });

    reboard::infrastructure::UiProcessController ui(uiBinaryPath());
    const auto shutdownRequested = [] { return gShutdownRequested != 0; };

    while (!shutdownRequested()) {
        const auto state = useCases.refreshForegroundState().execute();

        if (state == reboard::application::ForegroundState::Running) {
            if (homeRequested.exchange(false)) {
                try {
                    useCases.closeForegroundApplication().execute();
                } catch (const std::exception& exception) {
                    std::cerr << "reboard: failed to close the foreground application: "
                              << exception.what() << std::endl;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            continue;
        }

        // Nothing (left) on screen: show the launcher UI and wait for a choice.
        homeRequested.store(false);
        std::optional<std::string> directive;
        try {
            directive = ui.runUntilExit(shutdownRequested);
        } catch (const std::exception& exception) {
            std::cerr << "reboard: failed to run the launcher UI: " << exception.what()
                      << std::endl;
        }
        if (shutdownRequested()) {
            break;
        }
        if (directive) {
            try {
                const auto result = useCases.launchApplication().execute(
                    reboard::domain::ApplicationId(*directive));
                if (result != reboard::application::LaunchResult::Launched) {
                    std::cerr << "reboard: unknown application " << *directive << std::endl;
                }
            } catch (const std::exception& exception) {
                std::cerr << "reboard: failed to launch " << *directive << ": "
                          << exception.what() << std::endl;
            }
            homeRequested.store(false);  // Drop gestures made while the UI was up.
        } else {
            // The UI exited without choosing anything (crash?): brief backoff.
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    stopTouchThread.store(true);
    touchThread.join();
    return 0;
}
