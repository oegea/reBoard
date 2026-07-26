#pragma once

#include <vector>

#include "domain/repositories/ApplicationRepository.h"

namespace reboard::infrastructure {

// Provides the minimal set of fixed applications that must always exist on
// the board. The only built-in entry is "Notebooks", which returns to the
// stock device UI (the xochitl systemd service). Nothing is embedded: the
// entry merely points at an external systemd unit.
class BuiltInApplicationRepository : public domain::ApplicationRepository {
public:
    std::vector<domain::Application> findAll() const override {
        return {domain::Application(domain::ApplicationId("xochitl"),
                                    domain::ApplicationName("Notebooks"),
                                    domain::LaunchTarget::systemdUnit("xochitl"),
                                    "qrc:/icons/notebooks.png", /*pinnedToDock=*/true,
                                    /*showReturnHint=*/true),
                domain::Application(domain::ApplicationId("settings"),
                                    domain::ApplicationName("Settings"),
                                    domain::LaunchTarget::process({"/home/root/reboard-settings"}),
                                    "qrc:/icons/settings.png", /*pinnedToDock=*/true),
                domain::Application(domain::ApplicationId("store"),
                                    domain::ApplicationName("App Store"),
                                    domain::LaunchTarget::process({"/home/root/reboard-store"}),
                                    "qrc:/icons/store.png", /*pinnedToDock=*/true)};
    }
};

}  // namespace reboard::infrastructure
