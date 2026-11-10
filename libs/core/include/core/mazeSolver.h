#pragma once

#include "core/mazeGrid.h"

#include <vector>

// Stands in a distance table for a cell that cannot be reached.
inline constexpr int unreachableDistance = -1;

std::vector<int> distancesTo(const MazeGrid& maze, Position target, bool doorsCountAsOpen);

// Both ends included. An empty result means there is no way at all.
std::vector<Position> shortestPath(const MazeGrid& maze, Position from, Position to, bool doorsCountAsOpen);

bool isSolvableWithoutKeys(const MazeGrid& maze);
