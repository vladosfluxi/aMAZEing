#include "core/mazeGenerator.h"

#include "core/mazeSolver.h"

#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

static constexpr int minStepCost = 1;
static constexpr int maxStepCost = 9;

// a door is only worth it when it saves at least this many steps
static constexpr int minStepsSavedByDoor = 6;

struct DoorCandidate
{
    Position position;
    Direction direction;
    int stepsSaved;
};

static void carvePassages(MazeGrid& maze, std::mt19937& randomEngine)
{
    std::vector<bool> isVisited(static_cast<std::size_t>(maze.cellCount()), false);
    std::vector<Position> cellStack;

    const Position firstCell = maze.startPosition();
    isVisited[static_cast<std::size_t>(maze.indexOfPosition(firstCell))] = true;
    cellStack.push_back(firstCell);
