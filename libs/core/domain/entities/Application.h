#pragma once

#include <string>

#include "domain/valueobjects/ApplicationId.h"
#include "domain/valueobjects/ApplicationName.h"
#include "domain/valueobjects/LaunchTarget.h"

namespace reboard::domain {

// Immutable entity representing an application that can be launched from the
// board. Identity is defined by the ApplicationId.
class Application {
public:
    Application(ApplicationId id, ApplicationName name, LaunchTarget launchTarget,
                std::string iconPath = "", bool pinnedToDock = false,
                bool showReturnHint = false, bool removable = false)
        : id_(std::move(id)),
          name_(std::move(name)),
          launchTarget_(std::move(launchTarget)),
          iconPath_(std::move(iconPath)),
          pinnedToDock_(pinnedToDock),
          showReturnHint_(showReturnHint),
          removable_(removable) {}

    const ApplicationId& id() const noexcept { return id_; }
    const ApplicationName& name() const noexcept { return name_; }
    const LaunchTarget& launchTarget() const noexcept { return launchTarget_; }
    const std::string& iconPath() const noexcept { return iconPath_; }
    bool pinnedToDock() const noexcept { return pinnedToDock_; }
    // Whether the launcher should explain the return gesture before opening
    // (used for the stock UI, which fully takes over the device).
    bool showReturnHint() const noexcept { return showReturnHint_; }
    // Store-installed applications can be uninstalled from the board;
    // base system entries and hand-installed manifests cannot (ADR-0006).
    bool removable() const noexcept { return removable_; }

    // Entities compare by identity.
    bool operator==(const Application& other) const noexcept { return id_ == other.id_; }
    bool operator!=(const Application& other) const noexcept { return !(*this == other); }

private:
    ApplicationId id_;
    ApplicationName name_;
    LaunchTarget launchTarget_;
    std::string iconPath_;
    bool pinnedToDock_;
    bool showReturnHint_;
    bool removable_;
};

}  // namespace reboard::domain
