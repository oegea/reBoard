#pragma once

#include <string>

namespace reboard::infrastructure {

// Package manifests use the {APP_DIR} token so they never hardcode where a
// package lands on a device. The installer expands it to the final install
// directory when registering the manifest.
std::string expandAppDirToken(const std::string& manifestContent, const std::string& appDirectory);

}  // namespace reboard::infrastructure
