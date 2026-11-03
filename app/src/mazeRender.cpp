#include "app/mazeRenderer.h"

#include "app/uiTheme.h"

#include <algorithm>
#include <string>

static constexpr float minCellSizeForNumbers = 26.0f;

static Vector2 cellCorner(Position position, const MazeLayout &layout) {
  return Vector2{
      layout.originX + static_cast<float>(position.column) * layout.cellSize,
      layout.originY + static_cast<float>(position.row) * layout.cellSize};
}

static void drawWallLine(Vector2 start, Vector2 end, float thickness,
                         WallState wall) {
  const bool isDoor = wall == WallState::door;
  DrawLineEx(start, end, isDoor ? thickness * 1.4f : thickness,
             isDoor ? doorColor : wallColor);
}
MazeLayout calculateMazeLayout(const MazeGrid &maze, Rectangle viewport) {
  MazeLayout layout;
  if (maze.isEmpty() || viewport.width <= 0.0f || viewport.height <= 0.0f) {
    return layout;
  }

  const float widthPerColumn =
      viewport.width / static_cast<float>(maze.columnCount());
  const float heightPerRow =
      viewport.height / static_cast<float>(maze.rowCount());
  layout.cellSize = std::min(widthPerColumn, heightPerRow);

  const float drawnWidth =
      layout.cellSize * static_cast<float>(maze.columnCount());
  const float drawnHeight =
      layout.cellSize * static_cast<float>(maze.rowCount());
  layout.originX = viewport.x + (viewport.width - drawnWidth) * 0.5f;
  layout.originY = viewport.y + (viewport.height - drawnHeight) * 0.5f;
  return layout;
}

Vector2 cellCenter(Position position, const MazeLayout &layout) {
  const Vector2 corner = cellCorner(position, layout);
  return Vector2{corner.x + layout.cellSize * 0.5f,
                 corner.y + layout.cellSize * 0.5f};
}

std::optional<Position> cellAtPoint(Vector2 point, const MazeGrid &maze,
                                    const MazeLayout &layout) {
  if (maze.isEmpty() || layout.cellSize <= 0.0f) {
    return std::nullopt;
  }

  if (point.x < layout.originX || point.y < layout.originY) {
    return std::nullopt;
  }

  const Position position{
      static_cast<int>((point.y - layout.originY) / layout.cellSize),
      static_cast<int>((point.x - layout.originX) / layout.cellSize)};
  if (!maze.contains(position)) {
    return std::nullopt;
  }
  return position;
}
void drawCellFrame(Position position, const MazeLayout &layout) {
  const Vector2 corner = cellCorner(position, layout);
  DrawRectangleLinesEx(
      Rectangle{corner.x, corner.y, layout.cellSize, layout.cellSize},
      std::max(1.0f, layout.cellSize * 0.06f), highlightColor);
}

void drawMazeCells(const MazeGrid &maze, const MazeLayout &layout,
                   bool showCellNumbers) {
  const bool numbersFit =
      showCellNumbers && layout.cellSize >= minCellSizeForNumbers;
  const int numberFontSize = static_cast<int>(layout.cellSize * 0.42f);

  for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex) {
    const Position position = maze.positionOfIndex(cellIndex);
    const Cell &cell = maze.cellAtIndex(cellIndex);
    const Vector2 corner = cellCorner(position, layout);
    const bool isExit = position == maze.exitPosition();

    Color floorColor = cell.visitedByPlayer ? visitedCellColor : cellColor;
    if (isExit) {
      floorColor = exitColor;
    }

    DrawRectangleV(corner, Vector2{layout.cellSize, layout.cellSize},
                   floorColor);

    if (numbersFit) {
      const std::string cellText =
          isExit ? "EX" : std::to_string(cell.stepCost);
      const int textWidth = MeasureText(cellText.c_str(), numberFontSize);
      DrawText(
          cellText.c_str(),
          static_cast<int>(corner.x +
                           (layout.cellSize - static_cast<float>(textWidth)) *
                               0.5f),
          static_cast<int>(
              corner.y +
              (layout.cellSize - static_cast<float>(numberFontSize)) * 0.5f),
          numberFontSize, isExit ? backgroundColor : dimTextColor);
    }

    if (cell.hasKey) {
      const Vector2 center = cellCenter(position, layout);
      DrawCircleV(center, layout.cellSize * 0.18f, keyColor);
      DrawCircleV(Vector2{center.x + layout.cellSize * 0.16f, center.y},
                  layout.cellSize * 0.07f, keyColor);
    }
  }
}

void drawMazeWalls(const MazeGrid &maze, const MazeLayout &layout) {
  const float thickness = std::max(2.0f, layout.cellSize * 0.1f);

  for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex) {
    const Position position = maze.positionOfIndex(cellIndex);
    const Vector2 corner = cellCorner(position, layout);
    const Vector2 rightCorner{corner.x + layout.cellSize, corner.y};
    const Vector2 bottomCorner{corner.x, corner.y + layout.cellSize};
    const Vector2 farCorner{corner.x + layout.cellSize,
                            corner.y + layout.cellSize};

    const WallState northWall = maze.wallStateAt(position, Direction::north);
    if (northWall != WallState::open) {
      drawWallLine(corner, rightCorner, thickness, northWall);
    }

    const WallState westWall = maze.wallStateAt(position, Direction::west);
    if (westWall != WallState::open) {
      drawWallLine(corner, bottomCorner, thickness, westWall);
    }

    if (position.row == maze.rowCount() - 1) {
      drawWallLine(bottomCorner, farCorner, thickness, WallState::solid);
    }
    if (position.column == maze.columnCount() - 1) {
      drawWallLine(rightCorner, farCorner, thickness, WallState::solid);
    }
  }
}
