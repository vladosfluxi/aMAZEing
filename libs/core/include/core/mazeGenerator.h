#pragma once

#include "core/mazeGrid.h"

MazeGrid generateMaze(int rowCount, int columnCount, unsigned int randomSeed);

unsigned int makeRandomSeed();
