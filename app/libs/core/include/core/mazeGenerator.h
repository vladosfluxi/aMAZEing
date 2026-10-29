#pragma once

#include "core/mazeGrid.h"

// Builds a maze in which every cell can be reached without a key and the
// doors really do shorten the way out. Sides outside 10..50 are corrected.
// The same seed always gives the same maze again, which the tests rely on.
MazeGrid generateMaze(int rowCount, int columnCount, unsigned int randomSeed);

unsigned int makeRandomSeed();