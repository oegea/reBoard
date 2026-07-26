#pragma once

#include <optional>
#include <string>

namespace reboard::infrastructure {

// The launcher UI is a short-lived process: it draws the board, prints a
// directive to stdout and exits. The resident daemon parses that output once
// the UI is dead — which guarantees the e-paper display is free — and acts
// on it.
//
// Directive format, one per line: `launch:<application id>`.
// Returns the id of the LAST launch directive found, if any.
std::optional<std::string> parseLaunchDirective(const std::string& uiOutput);

}  // namespace reboard::infrastructure
