#include "domain/entities/Board.h"

#include <stdexcept>

namespace reboard::domain {

Board Board::organize(const std::vector<Application>& applications,
                      std::size_t pageCapacity, std::size_t dockCapacity) {
    if (pageCapacity == 0) {
        throw std::invalid_argument("Board page capacity must be greater than zero");
    }

    std::vector<Application> dock;
    std::vector<Application> gridApplications;

    for (const auto& application : applications) {
        if (application.pinnedToDock() && dock.size() < dockCapacity) {
            dock.push_back(application);
        } else {
            gridApplications.push_back(application);
        }
    }

    std::vector<std::vector<Application>> pages;
    for (const auto& application : gridApplications) {
        if (pages.empty() || pages.back().size() >= pageCapacity) {
            pages.emplace_back();
        }
        pages.back().push_back(application);
    }

    return Board(std::move(pages), std::move(dock));
}

}  // namespace reboard::domain
