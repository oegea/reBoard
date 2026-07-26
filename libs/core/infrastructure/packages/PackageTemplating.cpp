#include "infrastructure/packages/PackageTemplating.h"

namespace reboard::infrastructure {

std::string expandAppDirToken(const std::string& manifestContent,
                              const std::string& appDirectory) {
    static const std::string kToken = "{APP_DIR}";
    std::string result = manifestContent;
    std::size_t position = 0;
    while ((position = result.find(kToken, position)) != std::string::npos) {
        result.replace(position, kToken.size(), appDirectory);
        position += appDirectory.size();
    }
    return result;
}

}  // namespace reboard::infrastructure
