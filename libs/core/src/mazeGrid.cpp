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

