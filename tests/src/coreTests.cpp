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
