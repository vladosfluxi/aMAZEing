#pragma once

#include "core/mazeGrid.h"

#include <vector>

enum class MoveOutcome
{
    moved,         // a normal step
    unlockedDoor,  // a key was used and the door is now open
    reachedExit,   // the step ended on the exit cell: the game is won
    blockedByWall, // there is a solid wall in that direction
    blockedByDoor, // there is a door but the player has no key
    outsideMaze,   // that direction leaves the maze
    noActiveGame   // there is nothing to move, or the game is already won
};

struct GameStatistics
{
    int stepCount = 0;     // moves that really happened
    int keyCount = 0;      // keys currently carried (the inventory)
    int keysUsed = 0;      // doors opened with a key
    int collectedCost = 0; // sum of the numbers of the visited cells
    double elapsedSeconds = 0.0;
};

// What the information panel shows about a single cell.
struct CellReport
{
    Position position;
    int stepCost = 0;
    bool hasKey = false;
    bool visitedByPlayer = false;
    int openSides = 0;      // how many neighbours can be entered freely
    int doorSides = 0;      // how many neighbours are behind a door
    int distanceToExit = 0; // steps still needed, -1 when unreachable
};

class GameSession
{
public:
    void startGame(MazeGrid newMaze);

    // Puts back a run that was read from a save file.
    void restoreGame(MazeGrid savedMaze, Position savedPlayerPosition, const GameStatistics& savedStatistics);

    void clearGame();

    // Walls, doors, keys, the step counter and the win condition all happen here.
    MoveOutcome movePlayer(Direction direction);

    // The interface adds the time of the last frame. The session never reads a
    // clock itself, which is what lets a test say "42.5 seconds have passed".
    void addElapsedTime(double deltaSeconds);

    const MazeGrid& maze() const { return activeMaze; }
    Position playerPosition() const { return currentPlayerPosition; }
    const GameStatistics& statistics() const { return currentStatistics; }

    bool hasMaze() const { return !activeMaze.isEmpty(); }
    bool isFinished() const { return gameIsFinished; }
    bool isRunning() const { return hasMaze() && !gameIsFinished; }

    // The way out from where the player stands. Doors are only used when
    // there is a key for them.
    std::vector<Position> hintPath() const;

    CellReport inspectCell(Position position) const;
    CellReport inspectPlayerCell() const { return inspectCell(currentPlayerPosition); }

    int currentScore() const;

private:
    MazeGrid activeMaze;
    Position currentPlayerPosition;
    GameStatistics currentStatistics;
    bool gameIsFinished = false;
};

// A big maze and opened doors are worth points; steps, expensive cells and
// time cost them.
int calculateScore(int rowCount, int columnCount, const GameStatistics& statistics);

