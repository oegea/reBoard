#pragma once

#include <functional>
#include <optional>
#include <string>

namespace reboard::infrastructure {

// Spawns the short-lived launcher UI process, waits for it to exit and
// returns the application id it asked to launch (if any). The UI owning the
// e-paper display is dead by the time this returns, so it is always safe to
// start the next application afterwards.
class UiProcessController {
public:
    explicit UiProcessController(std::string uiBinaryPath);

    // Blocks until the UI exits. `shutdownRequested` is polled on signal
    // interruptions so the daemon can terminate the UI and leave cleanly.
    std::optional<std::string> runUntilExit(const std::function<bool()>& shutdownRequested);

private:
    std::string uiBinaryPath_;
};

}  // namespace reboard::infrastructure
