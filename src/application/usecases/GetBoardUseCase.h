#pragma once

#include <cstddef>

#include "domain/entities/Board.h"
#include "domain/repositories/ApplicationRepository.h"

namespace reboard::application {

// Builds the springboard-style layout (pages + dock) from the available
// applications.
class GetBoardUseCase {
public:
    explicit GetBoardUseCase(const domain::ApplicationRepository& applicationRepository,
                             std::size_t pageCapacity = domain::Board::kDefaultPageCapacity,
                             std::size_t dockCapacity = domain::Board::kDefaultDockCapacity)
        : applicationRepository_(applicationRepository),
          pageCapacity_(pageCapacity),
          dockCapacity_(dockCapacity) {}

    domain::Board execute() const {
        return domain::Board::organize(applicationRepository_.findAll(), pageCapacity_, dockCapacity_);
    }

private:
    const domain::ApplicationRepository& applicationRepository_;
    std::size_t pageCapacity_;
    std::size_t dockCapacity_;
};

}  // namespace reboard::application
