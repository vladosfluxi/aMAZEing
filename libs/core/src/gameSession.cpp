#include "core/gameSession.h"

#include "core/mazeSolver.h"

#include <algorithm>
#include <utility>

static constexpr int pointsPerCell = 10;
static constexpr int pointsLostPerStep = 2;
static constexpr int pointsPerOpenedDoor = 50;

void GameSession::startGame(MazeGrid newMaze)
{
    activeMaze = std::move(newMaze);
    currentPlayerPosition = activeMaze.startPosition();
    currentStatistics = GameStatistics{};
    gameIsFinished = false;

    if (hasMaze())
    {
        activeMaze.cellAt(currentPlayerPosition).visitedByPlayer = true;
    }
}

void GameSession::restoreGame(MazeGrid savedMaze, Position savedPlayerPosition,
    const GameStatistics& savedStatistics)
{
    activeMaze = std::move(savedMaze);
    currentStatistics = savedStatistics;

    // The file could have been edited by hand, so the position is checked.
    currentPlayerPosition =
        activeMaze.contains(savedPlayerPosition) ? savedPlayerPosition : activeMaze.startPosition();
    gameIsFinished = hasMaze() && currentPlayerPosition == activeMaze.exitPosition();

    if (hasMaze())
    {
        activeMaze.cellAt(currentPlayerPosition).visitedByPlayer = true;
    }
}

void GameSession::clearGame()
{
    activeMaze = MazeGrid{};
    currentPlayerPosition = Position{};
    currentStatistics = GameStatistics{};
    gameIsFinished = false;
}

MoveOutcome GameSession::movePlayer(Direction direction)
{
    if (!isRunning())
    {
        return MoveOutcome::noActiveGame;
    }

    const Position target = MazeGrid::neighbourOf(currentPlayerPosition, direction);
    if (!activeMaze.contains(target))
    {
        return MoveOutcome::outsideMaze;
    }

    const WallState wall = activeMaze.wallStateAt(currentPlayerPosition, direction);
    if (wall == WallState::solid)
    {
        return MoveOutcome::blockedByWall;
    }

    bool doorWasOpened = false;
    if (wall == WallState::door)
    {
        if (currentStatistics.keyCount <= 0)
        {
            return MoveOutcome::blockedByDoor;
        }

        --currentStatistics.keyCount;
        ++currentStatistics.keysUsed;
        activeMaze.setWallBetween(currentPlayerPosition, direction, WallState::open);
        doorWasOpened = true;
    }

    currentPlayerPosition = target;
    ++currentStatistics.stepCount;

    Cell& reachedCell = activeMaze.cellAt(currentPlayerPosition);
    currentStatistics.collectedCost += reachedCell.stepCost;
    reachedCell.visitedByPlayer = true;

    if (reachedCell.hasKey)
    {
        reachedCell.hasKey = false;
        ++currentStatistics.keyCount;
    }

    if (currentPlayerPosition == activeMaze.exitPosition())
    {
        gameIsFinished = true;
        return MoveOutcome::reachedExit;
    }

    return doorWasOpened ? MoveOutcome::unlockedDoor : MoveOutcome::moved;
}

void GameSession::addElapsedTime(double deltaSeconds)
{
    if (isRunning() && deltaSeconds > 0.0)
    {
        currentStatistics.elapsedSeconds += deltaSeconds;
    }
}

std::vector<Position> GameSession::hintPath() const
{
    if (!isRunning())
    {
        return std::vector<Position>{};
    }

    const bool mayUseDoors = currentStatistics.keyCount > 0;
    return shortestPath(activeMaze, currentPlayerPosition, activeMaze.exitPosition(), mayUseDoors);
}

CellReport GameSession::inspectCell(Position position) const
{
    CellReport report;
    if (!hasMaze() || !activeMaze.contains(position))
    {
        report.distanceToExit = unreachableDistance;
        return report;
    }

    const Cell& cell = activeMaze.cellAt(position);
    report.position = position;
    report.stepCost = cell.stepCost;
    report.hasKey = cell.hasKey;
    report.visitedByPlayer = cell.visitedByPlayer;

    for (const Direction direction : allDirections)
    {
        if (!activeMaze.contains(MazeGrid::neighbourOf(position, direction)))
        {
            continue;
        }

        const WallState wall = cell.walls[static_cast<std::size_t>(indexOfDirection(direction))];
        if (wall == WallState::open)
        {
            ++report.openSides;
        }
        else if (wall == WallState::door)
        {
            ++report.doorSides;
        }
    }

    const std::vector<int> distances = distancesTo(activeMaze, activeMaze.exitPosition(), false);
    report.distanceToExit = distances[static_cast<std::size_t>(activeMaze.indexOfPosition(position))];
    return report;
}

int GameSession::currentScore() const
{
    if (!hasMaze())
    {
        return 0;
    }
    return calculateScore(activeMaze.rowCount(), activeMaze.columnCount(), currentStatistics);
}

int calculateScore(int rowCount, int columnCount, const GameStatistics& statistics)
{
    const int sizeBonus = rowCount * columnCount * pointsPerCell;
    const int doorBonus = statistics.keysUsed * pointsPerOpenedDoor;
    const int stepPenalty = statistics.stepCount * pointsLostPerStep;
    const int costPenalty = statistics.collectedCost;
    const int timePenalty = static_cast<int>(statistics.elapsedSeconds);

    const int score = sizeBonus + doorBonus - stepPenalty - costPenalty - timePenalty;
    return std::max(0, score);
}