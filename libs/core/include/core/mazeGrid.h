#pragma once

#include "core/mazeTypes.h"

#include <vector>

// The maze: a rectangle of cells in one dynamic array.

class MazeGrid
{
public:
    MazeGrid() = default;
    MazeGrid(int rowCount, int columnCount);

    int rowCount() const { return gridRowCount; }
    int columnCount() const { return gridColumnCount; }
    int cellCount() const { return static_cast<int>(cellArray.size()); }
    bool isEmpty() const { return cellArray.empty(); }

    bool contains(Position position) const;

    Cell& cellAt(Position position);
    const Cell& cellAt(Position position) const;

    Cell& cellAtIndex(int index);
    const Cell& cellAtIndex(int index) const;

    int indexOfPosition(Position position) const;
    Position positionOfIndex(int index) const;

    Position startPosition() const { return Position{ 0, 0 }; }
    Position exitPosition() const { return Position{ gridRowCount - 1, gridColumnCount - 1 }; }

    WallState wallStateAt(Position position, Direction direction) const;

    void setWallBetween(Position position, Direction direction, WallState newState);

    // The result can lie outside the maze, so check contains() first.
    static Position neighbourOf(Position position, Direction direction);

private:
    int gridRowCount = 0;
    int gridColumnCount = 0;
    std::vector<Cell> cellArray;
};
