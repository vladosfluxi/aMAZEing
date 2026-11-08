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
    while (!cellStack.empty())
    {
        const Position current = cellStack.back();

        std::vector<Direction> freeDirections;
        for (const Direction direction : allDirections)
        {
            const Position neighbour = MazeGrid::neighbourOf(current, direction);
            if (maze.contains(neighbour) && !isVisited[static_cast<std::size_t>(maze.indexOfPosition(neighbour))])
            {
                freeDirections.push_back(direction);
            }
        }

        if (freeDirections.empty())
        {
            cellStack.pop_back(); // dead end, walk back
            continue;
        }

        std::uniform_int_distribution<std::size_t> pickDirection(0, freeDirections.size() - 1);
        const Direction chosenDirection = freeDirections[pickDirection(randomEngine)];
        const Position chosenNeighbour = MazeGrid::neighbourOf(current, chosenDirection);

        maze.setWallBetween(current, chosenDirection, WallState::open);
        isVisited[static_cast<std::size_t>(maze.indexOfPosition(chosenNeighbour))] = true;
        cellStack.push_back(chosenNeighbour);
    }
}


static void addShortcutDoors(MazeGrid& maze, int doorCount, std::mt19937& randomEngine)
{
    const std::vector<int> distances = distancesTo(maze, maze.exitPosition(), false);
    std::vector<DoorCandidate> candidates;

    // Only east and south, so every inner wall is looked at once.
    for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
    {
        const Position position = maze.positionOfIndex(cellIndex);
        for (const Direction direction : {Direction::east, Direction::south})
        {
            const Position neighbour = MazeGrid::neighbourOf(position, direction);
            if (!maze.contains(neighbour) || maze.wallStateAt(position, direction) != WallState::solid)
            {
                continue;
            }

            const int here = distances[static_cast<std::size_t>(cellIndex)];
            const int there = distances[static_cast<std::size_t>(maze.indexOfPosition(neighbour))];
            if (here == unreachableDistance || there == unreachableDistance)
            {
                continue;
            }

            const int stepsSaved = std::abs(here - there) - 1;
            if (stepsSaved >= minStepsSavedByDoor)
            {
                candidates.push_back(DoorCandidate{ position, direction, stepsSaved });
            }
        }
    }

    if (candidates.empty())
    {
        return;
    }

    
    std::shuffle(candidates.begin(), candidates.end(), randomEngine);
    std::stable_sort(candidates.begin(), candidates.end(),
        [](const DoorCandidate& first, const DoorCandidate& second)
        { return first.stepsSaved > second.stepsSaved; });

    const std::size_t doorsToPlace = std::min(static_cast<std::size_t>(doorCount), candidates.size());
    for (std::size_t doorIndex = 0; doorIndex < doorsToPlace; ++doorIndex)
    {
        maze.setWallBetween(candidates[doorIndex].position, candidates[doorIndex].direction, WallState::door);
    }
}

static void placeKeys(MazeGrid& maze, int keyCount, std::mt19937& randomEngine)
{
    std::vector<int> possibleIndexes;
    for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
    {
        const Position position = maze.positionOfIndex(cellIndex);
        if (!(position == maze.startPosition()) && !(position == maze.exitPosition()))
        {
            possibleIndexes.push_back(cellIndex);
        }
    }

    std::shuffle(possibleIndexes.begin(), possibleIndexes.end(), randomEngine);

    const std::size_t keysToPlace = std::min(static_cast<std::size_t>(keyCount), possibleIndexes.size());
    for (std::size_t keyIndex = 0; keyIndex < keysToPlace; ++keyIndex)
    {
        maze.cellAtIndex(possibleIndexes[keyIndex]).hasKey = true;
    }
}


static void assignStepCosts(MazeGrid& maze, std::mt19937& randomEngine)
{
    std::uniform_int_distribution<int> pickCost(minStepCost, maxStepCost);
    for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
    {
        maze.cellAtIndex(cellIndex).stepCost = pickCost(randomEngine);
    }

    maze.cellAt(maze.startPosition()).stepCost = 0; // the start is free
}

MazeGrid generateMaze(int rowCount, int columnCount, unsigned int randomSeed)
{
    MazeGrid maze(clampMazeSide(rowCount), clampMazeSide(columnCount));
    std::mt19937 randomEngine(randomSeed);

    // One key more than there are doors, so nobody can get stuck.
    const int doorCount = std::max(1, (maze.rowCount() + maze.columnCount()) / 12);
    const int keyCount = doorCount + 1;

    carvePassages(maze, randomEngine);
    addShortcutDoors(maze, doorCount, randomEngine);
    placeKeys(maze, keyCount, randomEngine);
    assignStepCosts(maze, randomEngine);

    return maze;
}

unsigned int makeRandomSeed()
{
    std::random_device seedSource;
    return seedSource();
}