#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/entities/Application.h"

namespace reboard::infrastructure {

// Parses reBoard application manifests. A manifest is a plain-text file with
// one key=value pair per line:
//
//   name=KOReader
//   exec=/opt/koreader/koreader.sh --some-flag
//   icon=/opt/etc/reboard/icons/koreader.png
//   dock=false
//
// Either `exec` (a command line) or `unit` (a systemd unit) is required.
// Lines starting with '#' and blank lines are ignored. Values in `exec` can
// be quoted with double quotes to preserve spaces.
class ManifestParser {
public:
    // `id` usually comes from the manifest file name (without extension).
    // Returns std::nullopt and fills `error` when the manifest is invalid.
    std::optional<domain::Application> parse(const std::string& id, const std::string& content,
                                             std::string* error = nullptr) const;

    // Splits a command line honoring double quotes. Exposed for testing.
    static std::optional<std::vector<std::string>> tokenizeCommandLine(const std::string& commandLine);
};

}  // namespace reboard::infrastructure
