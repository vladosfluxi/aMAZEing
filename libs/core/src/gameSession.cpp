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

