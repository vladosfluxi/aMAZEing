#pragma once

#include "core/mazeGrid.h"
#include "core/mazeTypes.h"

#include <optional>
#include <raylib.h>
#include <vector>

// Where the maze is drawn and how big one cell is.
struct MazeLayout
{
    float cellSize = 0.0f;
    float originX = 0.0f;
    float originY = 0.0f;
};

// The biggest cell size that still lets the whole maze fit, centred in the
// viewport. This is why a 10x10 and a 50x50 maze both look right.
MazeLayout calculateMazeLayout(const MazeGrid& maze, Rectangle viewport);

Vector2 cellCenter(Position position, const MazeLayout& layout);

// Empty when the point lies beside the maze.
std::optional<Position> cellAtPoint(Vector2 point, const MazeGrid& maze, const MazeLayout& layout);

void drawCellFrame(Position position, const MazeLayout& layout);
void drawMazeCells(const MazeGrid& maze, const MazeLayout& layout, bool showCellNumbers);
void drawMazeWalls(const MazeGrid& maze, const MazeLayout& layout);
void drawHintPath(const std::vector<Position>& hintPath, const MazeLayout& layout);

void drawPlayer(Position position, const MazeLayout& layout);
