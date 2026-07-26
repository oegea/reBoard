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
    // Applications from `removableManifestDirectories` (the store-managed
    // location, ADR-0006) are flagged removable; the rest never are.
    explicit FileApplicationRepository(std::vector<std::string> manifestDirectories,
                                       std::vector<std::string> removableManifestDirectories = {});

    std::vector<domain::Application> findAll() const override;

private:
    void loadDirectory(const std::string& directory, bool removable,
                       std::vector<domain::Application>& applications) const;

    std::vector<std::string> manifestDirectories_;
    std::vector<std::string> removableManifestDirectories_;
    ManifestParser parser_;
};

}  // namespace reboard::infrastructure
