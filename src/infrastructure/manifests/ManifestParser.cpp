#include "infrastructure/manifests/ManifestParser.h"

#include <map>
#include <sstream>

namespace reboard::infrastructure {

namespace {

std::string trim(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

void setError(std::string* error, const std::string& message) {
    if (error != nullptr) {
        *error = message;
    }
}

}  // namespace

std::optional<domain::Application> ManifestParser::parse(const std::string& id,
                                                         const std::string& content,
                                                         std::string* error) const {
    std::map<std::string, std::string> values;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed.front() == '#') {
            continue;
        }
        const auto separator = trimmed.find('=');
        if (separator == std::string::npos) {
            setError(error, "Invalid line (expected key=value): " + trimmed);
            return std::nullopt;
        }
        const std::string key = trim(trimmed.substr(0, separator));
        const std::string value = trim(trimmed.substr(separator + 1));
        if (key.empty()) {
            setError(error, "Empty key in line: " + trimmed);
            return std::nullopt;
        }
        values[key] = value;
    }

    const auto name = values.find("name");
    if (name == values.end() || name->second.empty()) {
        setError(error, "Manifest is missing the required 'name' key");
        return std::nullopt;
    }

    const bool hasExec = values.count("exec") > 0;
    const bool hasUnit = values.count("unit") > 0;
    if (hasExec == hasUnit) {
        setError(error, "Manifest must define exactly one of 'exec' or 'unit'");
        return std::nullopt;
    }

    bool dock = false;
    if (const auto dockValue = values.find("dock"); dockValue != values.end()) {
        if (dockValue->second == "true" || dockValue->second == "1") {
            dock = true;
        } else if (dockValue->second == "false" || dockValue->second == "0") {
            dock = false;
        } else {
            setError(error, "Invalid 'dock' value (expected true/false): " + dockValue->second);
            return std::nullopt;
        }
    }

    std::string iconPath;
    if (const auto icon = values.find("icon"); icon != values.end()) {
        iconPath = icon->second;
    }

    try {
        if (hasExec) {
            const auto argv = tokenizeCommandLine(values.at("exec"));
            if (!argv || argv->empty()) {
                setError(error, "Invalid 'exec' command line");
                return std::nullopt;
            }
            return domain::Application(domain::ApplicationId(id), domain::ApplicationName(name->second),
                                       domain::LaunchTarget::process(*argv), iconPath, dock);
        }
        return domain::Application(domain::ApplicationId(id), domain::ApplicationName(name->second),
                                   domain::LaunchTarget::systemdUnit(values.at("unit")), iconPath, dock);
    } catch (const std::exception& exception) {
        setError(error, exception.what());
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>> ManifestParser::tokenizeCommandLine(
    const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    bool tokenStarted = false;

    for (const char c : commandLine) {
        if (c == '"') {
            inQuotes = !inQuotes;
            tokenStarted = true;
            continue;
        }
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (tokenStarted) {
                tokens.push_back(current);
                current.clear();
                tokenStarted = false;
            }
            continue;
        }
        current += c;
        tokenStarted = true;
    }

    if (inQuotes) {
        return std::nullopt;  // Unterminated quote.
    }
    if (tokenStarted) {
        tokens.push_back(current);
    }
    return tokens;
}

}  // namespace reboard::infrastructure
