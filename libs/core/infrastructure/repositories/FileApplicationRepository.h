#pragma once

#include <string>
#include <vector>

#include "domain/repositories/ApplicationRepository.h"
#include "infrastructure/manifests/ManifestParser.h"

namespace reboard::infrastructure {

// Loads applications from `*.app` manifest files found in a list of
// directories. Missing directories are silently skipped; malformed manifests
// are skipped with a warning on stderr so one broken file never takes the
// whole board down.
class FileApplicationRepository : public domain::ApplicationRepository {
public:
    explicit FileApplicationRepository(std::vector<std::string> manifestDirectories);

    std::vector<domain::Application> findAll() const override;

private:
    std::vector<std::string> manifestDirectories_;
    ManifestParser parser_;
};

}  // namespace reboard::infrastructure
