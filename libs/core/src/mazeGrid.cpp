#include "core/mazeGrid.h"

#include <stdexcept>

MazeGrid::MazeGrid(int rowCount, int columnCount)
    : gridRowCount(rowCount),
    gridColumnCount(columnCount)
{
    if (rowCount <= 0 || columnCount <= 0)
    {
        throw std::invalid_argument("A maze needs at least one row and one column");
    }

    cellArray.resize(static_cast<std::size_t>(rowCount) * static_cast<std::size_t>(columnCount));
}

bool MazeGrid::contains(Position position) const
{
    return position.row >= 0 && position.row < gridRowCount &&
        position.column >= 0 && position.column < gridColumnCount;
}

int MazeGrid::indexOfPosition(Position position) const
{
    return position.row * gridColumnCount + position.column;
}

Position MazeGrid::positionOfIndex(int index) const
{
    return Position{ index / gridColumnCount, index % gridColumnCount };
}

Cell& MazeGrid::cellAt(Position position)
{
    return cellAtIndex(indexOfPosition(position));
}

const Cell& MazeGrid::cellAt(Position position) const
{
    return cellAtIndex(indexOfPosition(position));
}

Cell& MazeGrid::cellAtIndex(int index)
{
    // at() checks the index and throws instead of writing somewhere else.
    return cellArray.at(static_cast<std::size_t>(index));
}

const Cell& MazeGrid::cellAtIndex(int index) const
{
    return cellArray.at(static_cast<std::size_t>(index));
}

WallState MazeGrid::wallStateAt(Position position, Direction direction) const
{
    return cellAt(position).walls[static_cast<std::size_t>(indexOfDirection(direction))];
}

void MazeGrid::setWallBetween(Position position, Direction direction, WallState newState)
{
    cellAt(position).walls[static_cast<std::size_t>(indexOfDirection(direction))] = newState;

    const Position neighbour = neighbourOf(position, direction);
    if (contains(neighbour))
    {
        const Direction backwards = oppositeDirection(direction);
        cellAt(neighbour).walls[static_cast<std::size_t>(indexOfDirection(backwards))] = newState;
    }
}

Position MazeGrid::neighbourOf(Position position, Direction direction)
{
    switch (direction)
    {
    case Direction::north:
        return Position{ position.row - 1, position.column };
    case Direction::east:
        return Position{ position.row, position.column + 1 };
    case Direction::south:
        return Position{ position.row + 1, position.column };
    case Direction::west:
        return Position{ position.row, position.column - 1 };
    }
    return position;
}
