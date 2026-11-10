// The small types that the whole maze logic is built on.
#pragma once

#include <array>

inline constexpr int minMazeSide = 10;
inline constexpr int maxMazeSide = 50;

enum class Direction : int
{
    north = 0,
    east = 1,
    south = 2,
    west = 3
};

inline constexpr int directionCount = 4;

inline constexpr std::array<Direction, directionCount> allDirections{
    Direction::north, Direction::east, Direction::south, Direction::west };

inline constexpr int indexOfDirection(Direction direction)
{
    return static_cast<int>(direction);
}

inline constexpr Direction oppositeDirection(Direction direction)
{
    switch (direction)
    {
    case Direction::north:
        return Direction::south;
    case Direction::east:
        return Direction::west;
    case Direction::south:
        return Direction::north;
    case Direction::west:
        return Direction::east;
    }
    return Direction::north;
}

// Row 0, column 0 is the top left corner, where the player starts.
struct Position
{
    int row = 0;
    int column = 0;
};

inline constexpr bool operator==(Position first, Position second)
{
    return first.row == second.row && first.column == second.column;
}

// A door can only be passed with a key.
enum class WallState : int
{
    open = 0,
    solid = 1,
    door = 2
};

struct Cell
{
    std::array<WallState, directionCount> walls{
        WallState::solid, WallState::solid, WallState::solid, WallState::solid };
    bool hasKey = false;
    bool visitedByPlayer = false;
    int stepCost = 1; // the number shown inside the cell (1..9)
};

inline constexpr bool isValidMazeSide(int side)
{
    return side >= minMazeSide && side <= maxMazeSide;
}

inline constexpr int clampMazeSide(int side)
{
    if (side < minMazeSide)
    {
        return minMazeSide;
    }
    if (side > maxMazeSide)
    {
        return maxMazeSide;
    }
    return side;
}
