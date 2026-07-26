#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "domain/entities/Board.h"

using namespace reboard::domain;

namespace {

Application makeApp(const std::string& id, bool dock = false) {
    return Application(ApplicationId(id), ApplicationName("App " + id),
                       LaunchTarget::process({"/bin/" + id}), "", dock);
}

std::vector<Application> makeApps(std::size_t count, bool dock = false) {
    std::vector<Application> apps;
    for (std::size_t i = 0; i < count; ++i) {
        apps.push_back(makeApp("app" + std::to_string(i), dock));
    }
    return apps;
}

}  // namespace

TEST(BoardTest, EmptyInputProducesEmptyBoard) {
    const auto board = Board::organize({});
    EXPECT_TRUE(board.pages().empty());
    EXPECT_TRUE(board.dock().empty());
}

TEST(BoardTest, FillsPagesInOrder) {
    const auto board = Board::organize(makeApps(5), /*pageCapacity=*/3, /*dockCapacity=*/0);
    ASSERT_EQ(board.pages().size(), 2u);
    EXPECT_EQ(board.pages()[0].size(), 3u);
    EXPECT_EQ(board.pages()[1].size(), 2u);
    EXPECT_EQ(board.pages()[0][0].id().value(), "app0");
    EXPECT_EQ(board.pages()[1][1].id().value(), "app4");
}

TEST(BoardTest, PinnedApplicationsGoToTheDock) {
    std::vector<Application> apps = {makeApp("a"), makeApp("pinned", true), makeApp("b")};
    const auto board = Board::organize(apps);
    ASSERT_EQ(board.dock().size(), 1u);
    EXPECT_EQ(board.dock()[0].id().value(), "pinned");
    ASSERT_EQ(board.pages().size(), 1u);
    EXPECT_EQ(board.pages()[0].size(), 2u);
}

TEST(BoardTest, DockOverflowFallsBackToPages) {
    const auto board = Board::organize(makeApps(6, /*dock=*/true), /*pageCapacity=*/24,
                                       /*dockCapacity=*/4);
    EXPECT_EQ(board.dock().size(), 4u);
    ASSERT_EQ(board.pages().size(), 1u);
    EXPECT_EQ(board.pages()[0].size(), 2u);
}

TEST(BoardTest, ExactPageBoundaryDoesNotCreateEmptyPage) {
    const auto board = Board::organize(makeApps(24), /*pageCapacity=*/24, /*dockCapacity=*/0);
    EXPECT_EQ(board.pages().size(), 1u);
}

TEST(BoardTest, RejectsZeroPageCapacity) {
    EXPECT_THROW(Board::organize(makeApps(1), 0, 4), std::invalid_argument);
}
