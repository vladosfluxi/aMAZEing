#include "core/mazeSolver.h"

#include <queue>

static bool canStep(const MazeGrid& maze, Position position, Direction direction, bool doorsCountAsOpen)
{
    const Position neighbour = MazeGrid::neighbourOf(position, direction);
    if (!maze.contains(neighbour))
    {
        return false;
    }

    const WallState wall = maze.wallStateAt(position, direction);
    if (wall == WallState::open)
    {
        return true;
    }
    return wall == WallState::door && doorsCountAsOpen;
}

std::vector<int> distancesTo(const MazeGrid& maze, Position target, bool doorsCountAsOpen)
{
    std::vector<int> distances(static_cast<std::size_t>(maze.cellCount()), unreachableDistance);
    if (maze.isEmpty() || !maze.contains(target))
    {
        return distances;
    }

    
    std::queue<Position> cellsToVisit;
    distances[static_cast<std::size_t>(maze.indexOfPosition(target))] = 0;
    cellsToVisit.push(target);