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
