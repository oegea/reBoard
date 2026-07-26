#pragma once

#include <vector>

#include "domain/entities/Application.h"

namespace reboard::domain {

// Source of the applications available on the board. Implemented by the
// infrastructure layer (e.g. manifest files on disk).
class ApplicationRepository {
public:
    virtual ~ApplicationRepository() = default;

    virtual std::vector<Application> findAll() const = 0;
};

}  // namespace reboard::domain
