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

    while (!cellsToVisit.empty())
    {
        const Position current = cellsToVisit.front();
        cellsToVisit.pop();
        const int currentDistance = distances[static_cast<std::size_t>(maze.indexOfPosition(current))];

        for (const Direction direction : allDirections)
        {
            if (!canStep(maze, current, direction, doorsCountAsOpen))
            {
                continue;
            }

            const Position neighbour = MazeGrid::neighbourOf(current, direction);
            const std::size_t neighbourIndex = static_cast<std::size_t>(maze.indexOfPosition(neighbour));
            if (distances[neighbourIndex] == unreachableDistance)
            {
                distances[neighbourIndex] = currentDistance + 1;
                cellsToVisit.push(neighbour);
            }
        }
    }

    return distances;
}

std::vector<Position> shortestPath(const MazeGrid& maze, Position from, Position to, bool doorsCountAsOpen)
{
    std::vector<Position> path;
    if (!maze.contains(from) || !maze.contains(to))
    {
        return path;
    }

    // Measured from the target, so the way can be built by always stepping to
    // a neighbour that is one closer.
    const std::vector<int> distances = distancesTo(maze, to, doorsCountAsOpen);
    if (distances[static_cast<std::size_t>(maze.indexOfPosition(from))] == unreachableDistance)
    {
        return path;
    }

    Position current = from;
    path.push_back(current);
    while (!(current == to))
    {
        const int currentDistance = distances[static_cast<std::size_t>(maze.indexOfPosition(current))];
        for (const Direction direction : allDirections)
        {
            if (!canStep(maze, current, direction, doorsCountAsOpen))
            {
                continue;
            }

            const Position neighbour = MazeGrid::neighbourOf(current, direction);
            if (distances[static_cast<std::size_t>(maze.indexOfPosition(neighbour))] == currentDistance - 1)
            {
                current = neighbour;
                path.push_back(current);
                break;
            }
        }
    }

    return path;
}

bool isSolvableWithoutKeys(const MazeGrid& maze)
{
    if (maze.isEmpty())
    {
        return false;
    }

    const std::vector<int> distances = distancesTo(maze, maze.exitPosition(), false);
    const int startIndex = maze.indexOfPosition(maze.startPosition());
    return distances[static_cast<std::size_t>(startIndex)] != unreachableDistance;
}