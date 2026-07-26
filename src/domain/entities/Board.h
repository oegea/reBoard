#pragma once

#include <cstddef>
#include <vector>

#include "domain/entities/Application.h"

namespace reboard::domain {

// Immutable aggregate describing how applications are distributed across the
// launcher: a fixed dock at the bottom plus as many paginated grids as needed,
// in the spirit of the classic iOS springboard layout.
class Board {
public:
    static constexpr std::size_t kDefaultPageCapacity = 24;  // 4 columns x 6 rows
    static constexpr std::size_t kDefaultDockCapacity = 4;

    // Distributes applications: dock-pinned applications fill the dock until
    // capacity, everything else fills pages in order. Dock overflow falls back
    // to the pages so no application is ever lost.
    static Board organize(const std::vector<Application>& applications,
                          std::size_t pageCapacity = kDefaultPageCapacity,
                          std::size_t dockCapacity = kDefaultDockCapacity);

    const std::vector<std::vector<Application>>& pages() const noexcept { return pages_; }
    const std::vector<Application>& dock() const noexcept { return dock_; }

private:
    Board(std::vector<std::vector<Application>> pages, std::vector<Application> dock)
        : pages_(std::move(pages)), dock_(std::move(dock)) {}

    std::vector<std::vector<Application>> pages_;
    std::vector<Application> dock_;
};

}  // namespace reboard::domain
