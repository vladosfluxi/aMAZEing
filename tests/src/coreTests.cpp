// Tests for the rules of the game (the static library labyrinthCore).
#include "pch.h"

#include "core/gameSession.h"
#include "core/mazeGenerator.h"
#include "core/mazeGrid.h"
#include "core/mazeSolver.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// A small maze that is built by hand, so every test knows exactly where the
// walls, the doors and the keys are:
//
//   (0,0) - (0,1) - (0,2)      the first row is one open corridor
//     |                        (1,0) hangs below (0,0) and holds a key
//   (1,0)   door   (1,2)       a door connects (1,0) and (1,1)
static MazeGrid makeTestMaze()
{
    MazeGrid maze(2, 3);

    maze.setWallBetween(Position{0, 0}, Direction::east, WallState::open);
    maze.setWallBetween(Position{0, 1}, Direction::east, WallState::open);
    maze.setWallBetween(Position{0, 0}, Direction::south, WallState::open);
    maze.setWallBetween(Position{0, 2}, Direction::south, WallState::open);
    maze.setWallBetween(Position{1, 0}, Direction::east, WallState::door);

    for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
    {
        maze.cellAtIndex(cellIndex).stepCost = 1;
    }
    maze.cellAt(Position{1, 0}).hasKey = true;

    return maze;
}

// Walks one step of a hint path and says in which direction it went.
static Direction directionBetween(Position from, Position to)
{
    if (to.row == from.row + 1)
    {
        return Direction::south;
    }
    if (to.row == from.row - 1)
    {
        return Direction::north;
    }
    if (to.column == from.column + 1)
    {
        return Direction::east;
    }
    return Direction::west;
}

TEST_CLASS(MazeGridTests)
{
public:
    TEST_METHOD(mazeKeepsTheSizeItWasBuiltWith)
    {
        MazeGrid maze(12, 15);

        Assert::AreEqual(12, maze.rowCount(), L"the maze keeps its number of rows");
        Assert::AreEqual(15, maze.columnCount(), L"the maze keeps its number of columns");
        Assert::AreEqual(180, maze.cellCount(), L"the dynamic array holds rows times columns cells");
        Assert::IsFalse(maze.isEmpty(), L"a maze with cells is not empty");
    }

    TEST_METHOD(cellsInsideAndOutsideAreToldApart)
    {
        MazeGrid maze(12, 15);

        Assert::IsTrue(maze.contains(Position{0, 0}), L"the start corner belongs to the maze");
        Assert::IsTrue(maze.contains(Position{11, 14}), L"the exit corner belongs to the maze");
        Assert::IsFalse(maze.contains(Position{-1, 0}), L"a negative row is outside of the maze");
        Assert::IsFalse(maze.contains(Position{12, 0}), L"a row behind the last one is outside");
    }

    TEST_METHOD(startAndExitSitInTheRightCorners)
    {
        MazeGrid maze(12, 15);

        Assert::IsTrue(maze.startPosition() == Position{0, 0}, L"the start is the top left corner");
        Assert::IsTrue(maze.exitPosition() == Position{11, 14}, L"the exit is the bottom right corner");
    }

    TEST_METHOD(indexAndPositionDescribeTheSameCell)
    {
        MazeGrid maze(12, 15);

        Assert::AreEqual(50, maze.indexOfPosition(maze.positionOfIndex(50)),
                         L"index and position fit together");
        Assert::AreEqual(0, maze.indexOfPosition(maze.startPosition()), L"the start cell is the first one");
    }

    TEST_METHOD(aWallBelongsToBothOfItsCells)
    {
        MazeGrid maze(12, 15);
        const Position someCell{4, 7};

        maze.setWallBetween(someCell, Direction::east, WallState::open);

        Assert::IsTrue(maze.wallStateAt(someCell, Direction::east) == WallState::open,
                       L"the opened wall is open on the first cell");
        Assert::IsTrue(maze.wallStateAt(Position{4, 8}, Direction::west) == WallState::open,
                       L"the opened wall is open on the neighbour as well");
    }

    TEST_METHOD(aCopiedMazeOwnsItsOwnCells)
    {
        MazeGrid original(10, 12);
        original.setWallBetween(Position{3, 3}, Direction::east, WallState::door);
        original.cellAt(Position{3, 3}).stepCost = 7;

        MazeGrid copiedMaze = original;
        copiedMaze.cellAt(Position{3, 3}).stepCost = 1;

        Assert::AreEqual(7, original.cellAt(Position{3, 3}).stepCost, L"a copy does not share its cells");
        Assert::IsTrue(copiedMaze.wallStateAt(Position{3, 3}, Direction::east) == WallState::door,
                       L"a copy keeps the walls of the maze it came from");
        Assert::AreEqual(10, copiedMaze.rowCount(), L"a copy keeps the size of the maze");
    }

    TEST_METHOD(aMovedMazeTakesItsCellsWithIt)
    {
        MazeGrid original(10, 12);
        MazeGrid movedMaze = std::move(original);

        Assert::AreEqual(10, movedMaze.rowCount(), L"the moved maze keeps the rows");
        Assert::AreEqual(12, movedMaze.columnCount(), L"the moved maze keeps the columns");
        Assert::AreEqual(120, movedMaze.cellCount(), L"the moved maze keeps all of its cells");
    }
};
REGISTER_TEST_CLASS(MazeGridTests);
