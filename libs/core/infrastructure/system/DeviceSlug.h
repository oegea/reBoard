#pragma once

#include <string>

namespace reboard::infrastructure {

// Maps the kernel machine name (/sys/devices/soc0/machine) to the device
// slug used by the store catalog downloads (ADR-0006).
std::string deviceSlugFromMachineName(const std::string& machineName);

// Reads the machine name from sysfs; honors the REBOARD_DEVICE override.
std::string currentDeviceSlug();

}  // namespace reboard::infrastructure
