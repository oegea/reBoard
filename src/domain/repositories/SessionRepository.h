#pragma once

#include <optional>

#include "domain/valueobjects/ForegroundApplication.h"

namespace reboard::domain {

// Tracks which application currently owns the screen. Implemented by the
// infrastructure layer (in-memory for a single resident launcher process).
class SessionRepository {
public:
    virtual ~SessionRepository() = default;

    virtual void setForeground(const ForegroundApplication& foreground) = 0;
    virtual std::optional<ForegroundApplication> foreground() const = 0;
    virtual void clear() = 0;
};

}  // namespace reboard::domain
