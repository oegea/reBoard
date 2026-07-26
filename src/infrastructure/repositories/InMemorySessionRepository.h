#pragma once

#include <mutex>
#include <optional>

#include "domain/repositories/SessionRepository.h"

namespace reboard::infrastructure {

// Foreground tracking for a single resident launcher process. Thread-safe
// because use cases are reached both from the UI thread and the touch
// monitoring thread.
class InMemorySessionRepository : public domain::SessionRepository {
public:
    void setForeground(const domain::ForegroundApplication& foreground) override {
        const std::lock_guard<std::mutex> lock(mutex_);
        foreground_ = foreground;
    }

    std::optional<domain::ForegroundApplication> foreground() const override {
        const std::lock_guard<std::mutex> lock(mutex_);
        return foreground_;
    }

    void clear() override {
        const std::lock_guard<std::mutex> lock(mutex_);
        foreground_.reset();
    }

private:
    mutable std::mutex mutex_;
    std::optional<domain::ForegroundApplication> foreground_;
};

}  // namespace reboard::infrastructure
