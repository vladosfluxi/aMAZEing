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
