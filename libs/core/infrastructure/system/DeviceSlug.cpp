#include "infrastructure/system/DeviceSlug.h"

#include <cstdlib>
#include <fstream>

namespace reboard::infrastructure {

std::string deviceSlugFromMachineName(const std::string& machineName) {
    if (machineName.find("reMarkable 1") != std::string::npos ||
        machineName.find("reMarkable Prototype 1") != std::string::npos) {
        return "rm1";
    }
    if (machineName.find("reMarkable 2") != std::string::npos) {
        return "rm2";
    }
    if (machineName.find("Ferrari") != std::string::npos ||
        machineName.find("ferrari") != std::string::npos) {
        return "ferrari";
    }
    // Default to the primary supported device rather than failing: a wrong
    // slug only means "no download available", never a broken install.
    return "rm2";
}

std::string currentDeviceSlug() {
    if (const char* override = std::getenv("REBOARD_DEVICE"); override != nullptr) {
        return override;
    }
    std::ifstream machineFile("/sys/devices/soc0/machine");
    std::string machineName;
    if (machineFile) {
        std::getline(machineFile, machineName);
    }
    return deviceSlugFromMachineName(machineName);
}

}  // namespace reboard::infrastructure
