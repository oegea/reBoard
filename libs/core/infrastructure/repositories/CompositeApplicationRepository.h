#pragma once

#include <algorithm>
#include <vector>

#include "domain/repositories/ApplicationRepository.h"

namespace reboard::infrastructure {

// Concatenates several application sources. When two sources define the same
// application id, the first source wins — list user manifests before the
// built-ins so users can customize built-in entries.
class CompositeApplicationRepository : public domain::ApplicationRepository {
public:
    explicit CompositeApplicationRepository(
        std::vector<const domain::ApplicationRepository*> sources)
        : sources_(std::move(sources)) {}

    std::vector<domain::Application> findAll() const override {
        std::vector<domain::Application> applications;
        for (const auto* source : sources_) {
            for (const auto& application : source->findAll()) {
                const bool alreadyPresent =
                    std::any_of(applications.begin(), applications.end(),
                                [&application](const domain::Application& existing) {
                                    return existing.id() == application.id();
                                });
                if (!alreadyPresent) {
                    applications.push_back(application);
                }
            }
        }
        return applications;
    }

private:
    std::vector<const domain::ApplicationRepository*> sources_;
};

}  // namespace reboard::infrastructure
