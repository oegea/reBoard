#include "infrastructure/processes/UiDirective.h"

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

}  // namespace

std::optional<std::string> parseLaunchDirective(const std::string& uiOutput) {
    constexpr const char* kPrefix = "launch:";
    std::optional<std::string> result;

    std::istringstream stream(uiOutput);
    std::string line;
    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (trimmed.rfind(kPrefix, 0) != 0) {
            continue;
        }
        const std::string id = trim(trimmed.substr(std::string(kPrefix).size()));
        if (!id.empty()) {
            result = id;
        }
    }
    return result;
}

}  // namespace reboard::infrastructure
