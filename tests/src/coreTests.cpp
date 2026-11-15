// Tests for the rules (labyrinthCore). The core library knows nothing about
// windows and nothing about files, so all of it can be tested without a screen.
#include "pch.h"

#include "core/gameSession.h"
#include "core/mazeGenerator.h"
#include "core/mazeGrid.h"
#include "core/mazeSolver.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Built by hand so every test knows where the walls, the door and the key are:
//
//   (0,0) - (0,1) - (0,2)      the first row is one open corridor
//     |                        (1,0) hangs under (0,0) and holds a key
//   (1,0)   door   (1,2)       a door between (1,0) and (1,1)
static MazeGrid makeTestMaze()
{
    MazeGrid maze(2, 3);

    maze.setWallBetween(Position{0, 0}, Direction::east, WallState::open);
    maze.setWallBetween(Position{0, 1}, Direction::east, WallState::open);
    maze.setWallBetween(Position{0, 0}, Direction::south, WallState::open);
    maze.setWallBetween(Position{0, 2}, Direction::south, WallState::open);
    maze.setWallBetween(Position{1, 0}, Direction::east, WallState::door);

    for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
    {
        maze.cellAtIndex(cellIndex).stepCost = 1;
    }
    maze.cellAt(Position{1, 0}).hasKey = true;

    return maze;
}

static Direction directionBetween(Position from, Position to)
{
    if (to.row == from.row + 1)
    {
        return Direction::south;
    }
    if (to.row == from.row - 1)
    {
        return Direction::north;
    }
    if (to.column == from.column + 1)
    {
        return Direction::east;
    }
    return Direction::west;
}

TEST_CLASS(MazeGridTests)
{
public:
    TEST_METHOD(mazeKeepsTheSizeItWasBuiltWith)
    {
        MazeGrid maze(12, 15);

        Assert::AreEqual(12, maze.rowCount(), L"the maze keeps its number of rows");
        Assert::AreEqual(15, maze.columnCount(), L"the maze keeps its number of columns");
        Assert::AreEqual(180, maze.cellCount(), L"the dynamic array holds rows times columns cells");
        Assert::IsFalse(maze.isEmpty(), L"a maze with cells is not empty");
    }

    TEST_METHOD(cellsInsideAndOutsideAreToldApart)
    {
        MazeGrid maze(12, 15);

        Assert::IsTrue(maze.contains(Position{0, 0}), L"the start corner belongs to the maze");
        Assert::IsTrue(maze.contains(Position{11, 14}), L"the exit corner belongs to the maze");
        Assert::IsFalse(maze.contains(Position{-1, 0}), L"a negative row is outside of the maze");
        Assert::IsFalse(maze.contains(Position{12, 0}), L"a row behind the last one is outside");
    }

    TEST_METHOD(startAndExitSitInTheRightCorners)
    {
        MazeGrid maze(12, 15);

        Assert::IsTrue(maze.startPosition() == Position{0, 0}, L"the start is the top left corner");
        Assert::IsTrue(maze.exitPosition() == Position{11, 14}, L"the exit is the bottom right corner");
    }

    TEST_METHOD(indexAndPositionDescribeTheSameCell)
    {
        MazeGrid maze(12, 15);

        Assert::AreEqual(50, maze.indexOfPosition(maze.positionOfIndex(50)),
                         L"index and position fit together");
        Assert::AreEqual(0, maze.indexOfPosition(maze.startPosition()), L"the start cell is the first one");
    }

    TEST_METHOD(aWallBelongsToBothOfItsCells)
    {
        MazeGrid maze(12, 15);
        const Position someCell{4, 7};

        maze.setWallBetween(someCell, Direction::east, WallState::open);

        Assert::IsTrue(maze.wallStateAt(someCell, Direction::east) == WallState::open,
                       L"the opened wall is open on the first cell");
        Assert::IsTrue(maze.wallStateAt(Position{4, 8}, Direction::west) == WallState::open,
                       L"the opened wall is open on the neighbour as well");
    }

    TEST_METHOD(aCopiedMazeOwnsItsOwnCells)
    {
        MazeGrid original(10, 12);
        original.setWallBetween(Position{3, 3}, Direction::east, WallState::door);
        original.cellAt(Position{3, 3}).stepCost = 7;

        MazeGrid copiedMaze = original;
        copiedMaze.cellAt(Position{3, 3}).stepCost = 1;

        Assert::AreEqual(7, original.cellAt(Position{3, 3}).stepCost, L"a copy does not share its cells");
        Assert::IsTrue(copiedMaze.wallStateAt(Position{3, 3}, Direction::east) == WallState::door,
                       L"a copy keeps the walls of the maze it came from");
        Assert::AreEqual(10, copiedMaze.rowCount(), L"a copy keeps the size of the maze");
    }

    TEST_METHOD(aMovedMazeTakesItsCellsWithIt)
    {
        MazeGrid original(10, 12);
        MazeGrid movedMaze = std::move(original);

        Assert::AreEqual(10, movedMaze.rowCount(), L"the moved maze keeps the rows");
        Assert::AreEqual(12, movedMaze.columnCount(), L"the moved maze keeps the columns");
        Assert::AreEqual(120, movedMaze.cellCount(), L"the moved maze keeps all of its cells");
    }
};
REGISTER_TEST_CLASS(MazeGridTests);

TEST_CLASS(MazeSizeValidationTests)
{
public:
    TEST_METHOD(onlySidesFromTenToFiftyAreAllowed)
    {
        Assert::IsFalse(isValidMazeSide(9), L"9 is too small for a maze side");
        Assert::IsTrue(isValidMazeSide(10), L"10 is the smallest allowed side");
        Assert::IsTrue(isValidMazeSide(50), L"50 is the biggest allowed side");
        Assert::IsFalse(isValidMazeSide(51), L"51 is too big for a maze side");
    }

    TEST_METHOD(aWrongSideIsCorrected)
    {
        Assert::AreEqual(10, clampMazeSide(3), L"a side that is too small becomes 10");
        Assert::AreEqual(50, clampMazeSide(70), L"a side that is too big becomes 50");
        Assert::AreEqual(25, clampMazeSide(25), L"an allowed side stays as it is");
    }

    TEST_METHOD(theGeneratorNeverBuildsAnIllegalMaze)
    {
        const MazeGrid tinyMaze = generateMaze(2, 2, 123u);

        Assert::AreEqual(10, tinyMaze.rowCount(), L"the generator corrects a size that is too small");
        Assert::AreEqual(10, tinyMaze.columnCount(), L"the generator corrects both sides");
    }
};
REGISTER_TEST_CLASS(MazeSizeValidationTests);

TEST_CLASS(MazeGeneratorTests)
{
public:
    TEST_METHOD(everyMazeCanBeFinishedWithoutAKey)
    {
        const std::vector<int> sidesToTest{10, 17, 33, 50};
        for (const int side : sidesToTest)
        {
            const MazeGrid maze = generateMaze(side, side, 7u * static_cast<unsigned int>(side));

            Assert::AreEqual(side, maze.rowCount(), L"the generated maze has the wanted size");
            Assert::IsTrue(isSolvableWithoutKeys(maze), L"the maze can be finished without a single key");
        }
    }

    TEST_METHOD(everyCellOfTheMazeCanBeReached)
    {
        const MazeGrid maze = generateMaze(25, 25, 55u);
        const std::vector<int> distances = distancesTo(maze, maze.startPosition(), false);

        for (const int distance : distances)
        {
            Assert::AreNotEqual(unreachableDistance, distance, L"no cell of the maze is locked away");
        }
    }

    TEST_METHOD(theSameSeedBuildsTheSameMaze)
    {
        const MazeGrid firstMaze = generateMaze(20, 20, 4242u);
        const MazeGrid secondMaze = generateMaze(20, 20, 4242u);

        Assert::AreEqual(firstMaze.cellCount(), secondMaze.cellCount(), L"both mazes have the same size");
        for (int cellIndex = 0; cellIndex < firstMaze.cellCount(); ++cellIndex)
        {
            const Cell& firstCell = firstMaze.cellAtIndex(cellIndex);
            const Cell& secondCell = secondMaze.cellAtIndex(cellIndex);

            Assert::IsTrue(firstCell.walls == secondCell.walls, L"both mazes have the same walls");
            Assert::AreEqual(firstCell.stepCost, secondCell.stepCost, L"both mazes have the same numbers");
        }
    }

    TEST_METHOD(thereAreAtLeastAsManyKeysAsDoors)
    {
        const MazeGrid maze = generateMaze(30, 30, 99u);
        int doorCount = 0;
        int keyCount = 0;

        for (int cellIndex = 0; cellIndex < maze.cellCount(); ++cellIndex)
        {
            const Cell& cell = maze.cellAtIndex(cellIndex);
            if (cell.hasKey)
            {
                ++keyCount;
            }

            // only two sides, or every wall would be counted twice
            for (const Direction direction : {Direction::east, Direction::south})
            {
                if (cell.walls[static_cast<std::size_t>(indexOfDirection(direction))] == WallState::door)
                {
                    ++doorCount;
                }
            }
        }

        Assert::IsTrue(keyCount >= doorCount, L"the player can never be stuck in front of a door");
    }
};
REGISTER_TEST_CLASS(MazeGeneratorTests);

TEST_CLASS(MovementTests)
{
public:
    TEST_METHOD(aNewGameStartsInTheTopLeftCorner)
    {
        GameSession session;
        session.startGame(makeTestMaze());

        Assert::IsTrue(session.isRunning(), L"a started game is running");
        Assert::IsTrue(session.playerPosition() == Position{0, 0}, L"the player starts in the corner");
        Assert::AreEqual(0, session.statistics().stepCount, L"a new game has no steps yet");
    }

    TEST_METHOD(aStepOverTheBorderIsRefused)
    {
        GameSession session;
        session.startGame(makeTestMaze());

        Assert::IsTrue(session.movePlayer(Direction::north) == MoveOutcome::outsideMaze,
                       L"a step over the border is refused");
        Assert::AreEqual(0, session.statistics().stepCount, L"a refused step is not counted");
    }

    TEST_METHOD(aStepThroughAnOpenSideWorks)
    {
        GameSession session;
        session.startGame(makeTestMaze());

        Assert::IsTrue(session.movePlayer(Direction::east) == MoveOutcome::moved,
                       L"a step through an open side works");
        Assert::IsTrue(session.playerPosition() == Position{0, 1}, L"the player stands on the new cell");
        Assert::AreEqual(1, session.statistics().stepCount, L"the step was counted");
        Assert::AreEqual(1, session.statistics().collectedCost, L"the number of the cell was added");
    }

    TEST_METHOD(aSolidWallStopsThePlayer)
    {
        GameSession session;
        session.startGame(makeTestMaze());
        session.movePlayer(Direction::east);

        Assert::IsTrue(session.movePlayer(Direction::south) == MoveOutcome::blockedByWall,
                       L"a solid wall stops the player");
        Assert::IsTrue(session.playerPosition() == Position{0, 1}, L"a blocked step does not move anybody");
    }
};
REGISTER_TEST_CLASS(MovementTests);

TEST_CLASS(KeyAndDoorTests)
{
public:
    TEST_METHOD(walkingOverAKeyPutsItIntoTheInventory)
    {
        GameSession session;
        session.startGame(makeTestMaze());

        session.movePlayer(Direction::south); // onto the cell with the key

        Assert::AreEqual(1, session.statistics().keyCount, L"the key is in the inventory");
        Assert::IsFalse(session.maze().cellAt(Position{1, 0}).hasKey, L"the key is gone from the maze");
    }

    TEST_METHOD(aDoorCanBeOpenedWithAKey)
    {
        GameSession session;
        session.startGame(makeTestMaze());
        session.movePlayer(Direction::south);

        Assert::IsTrue(session.movePlayer(Direction::east) == MoveOutcome::unlockedDoor,
                       L"with a key the door can be opened");
        Assert::AreEqual(0, session.statistics().keyCount, L"opening a door uses one key");
        Assert::AreEqual(1, session.statistics().keysUsed, L"the opened door was counted");
        Assert::IsTrue(session.maze().wallStateAt(Position{1, 0}, Direction::east) == WallState::open,
                       L"an opened door stays open");
    }

    TEST_METHOD(aDoorBlocksWithoutAKey)
    {
        MazeGrid mazeWithoutKeys = makeTestMaze();
        mazeWithoutKeys.cellAt(Position{1, 0}).hasKey = false;

        GameSession session;
        session.startGame(std::move(mazeWithoutKeys));
        session.movePlayer(Direction::south);

        Assert::IsTrue(session.movePlayer(Direction::east) == MoveOutcome::blockedByDoor,
                       L"without a key the door blocks the way");
        Assert::IsTrue(session.playerPosition() == Position{1, 0}, L"a locked door does not move anybody");
    }
};
REGISTER_TEST_CLASS(KeyAndDoorTests);

TEST_CLASS(WinningAndScoreTests)
{
public:
    TEST_METHOD(theExitWinsTheGame)
    {
        GameSession session;
        session.startGame(makeTestMaze());

        session.movePlayer(Direction::east);
        session.movePlayer(Direction::east);

        Assert::IsTrue(session.movePlayer(Direction::south) == MoveOutcome::reachedExit,
                       L"the step onto the exit cell wins the game");
        Assert::IsTrue(session.isFinished(), L"the game is finished");
        Assert::IsFalse(session.isRunning(), L"a finished game is not running any more");
    }

    TEST_METHOD(aFinishedGameRefusesMoreSteps)
    {
        GameSession session;
        session.startGame(makeTestMaze());
        session.movePlayer(Direction::east);
        session.movePlayer(Direction::east);
        session.movePlayer(Direction::south);

        Assert::IsTrue(session.movePlayer(Direction::north) == MoveOutcome::noActiveGame,
                       L"a finished game refuses more steps");
    }

    TEST_METHOD(aShorterWayGivesMorePoints)
    {
        GameStatistics shortRun;
        shortRun.stepCount = 20;
        shortRun.collectedCost = 40;
        shortRun.elapsedSeconds = 30.0;

        GameStatistics longRun = shortRun;
        longRun.stepCount = 80;
        longRun.collectedCost = 160;

        Assert::IsTrue(calculateScore(20, 20, shortRun) > calculateScore(20, 20, longRun),
                       L"fewer steps and less cost give more points");
    }

    TEST_METHOD(thePointsNeverFallBelowZero)
    {
        GameStatistics terribleRun;
        terribleRun.stepCount = 100000;

        Assert::AreEqual(0, calculateScore(10, 10, terribleRun), L"the points never fall below zero");
    }

    TEST_METHOD(anOpenedDoorGivesABonus)
    {
        GameStatistics runWithoutDoor;
        runWithoutDoor.stepCount = 20;
        runWithoutDoor.collectedCost = 40;

        GameStatistics runWithDoor = runWithoutDoor;
        runWithDoor.keysUsed = 1;

        Assert::IsTrue(calculateScore(20, 20, runWithDoor) > calculateScore(20, 20, runWithoutDoor),
                       L"an opened door gives a bonus");
    }
};
REGISTER_TEST_CLASS(WinningAndScoreTests);

TEST_CLASS(HintPathTests)
{
public:
    TEST_METHOD(theHintStartsAtThePlayerAndEndsAtTheExit)
    {
        GameSession session;
        session.startGame(generateMaze(15, 15, 2026u));

        const std::vector<Position> path = session.hintPath();

        Assert::IsFalse(path.empty(), L"the hint finds a way out of the maze");
        Assert::IsTrue(path.front() == session.playerPosition(), L"the hint starts at the player");
        Assert::IsTrue(path.back() == session.maze().exitPosition(), L"the hint ends at the exit");
    }

    TEST_METHOD(theHintUsesOnlyStepsToANeighbourCell)
    {
        GameSession session;
        session.startGame(generateMaze(15, 15, 2026u));

        const std::vector<Position> path = session.hintPath();
        for (std::size_t stepIndex = 1; stepIndex < path.size(); ++stepIndex)
        {
            const Position previous = path[stepIndex - 1];
            const Position current = path[stepIndex];
            const int rowSteps = current.row > previous.row ? current.row - previous.row
                                                            : previous.row - current.row;
            const int columnSteps = current.column > previous.column ? current.column - previous.column
                                                                     : previous.column - current.column;
            Assert::AreEqual(1, rowSteps + columnSteps, L"every step goes to a neighbour cell");
        }
    }

    TEST_METHOD(followingTheHintWinsTheGame)
    {
        GameSession session;
        session.startGame(generateMaze(15, 15, 2026u));

        const std::vector<Position> path = session.hintPath();
        MoveOutcome lastOutcome = MoveOutcome::moved;
        for (std::size_t stepIndex = 1; stepIndex < path.size(); ++stepIndex)
        {
            lastOutcome = session.movePlayer(directionBetween(session.playerPosition(), path[stepIndex]));
        }

        Assert::IsTrue(lastOutcome == MoveOutcome::reachedExit, L"following the hint wins the game");
    }
};
REGISTER_TEST_CLASS(HintPathTests);

TEST_CLASS(RestoredRunTests)
{
public:
    TEST_METHOD(anImpossibleSavedPositionFallsBackToTheStart)
    {
        GameStatistics savedStatistics;
        savedStatistics.stepCount = 12;

        GameSession session;
        session.restoreGame(makeTestMaze(), Position{99, 99}, savedStatistics);

        Assert::IsTrue(session.playerPosition() == Position{0, 0}, L"an impossible position is corrected");
        Assert::AreEqual(12, session.statistics().stepCount, L"the saved step count is kept");
    }

    TEST_METHOD(aCorrectSavedPositionIsKept)
    {
        GameSession session;
        session.restoreGame(makeTestMaze(), Position{1, 2}, GameStatistics{});

        Assert::IsTrue(session.playerPosition() == Position{1, 2}, L"a correct saved position is kept");
    }

    TEST_METHOD(theReportOfACellIsRight)
    {
        GameSession session;
        session.restoreGame(makeTestMaze(), Position{1, 2}, GameStatistics{});

        const CellReport exitReport = session.inspectPlayerCell();
        Assert::IsTrue(exitReport.position == Position{1, 2}, L"the report describes the asked cell");
        Assert::AreEqual(0, exitReport.distanceToExit, L"the exit cell is zero steps away");

        const CellReport startReport = session.inspectCell(Position{0, 0});
        Assert::AreEqual(2, startReport.openSides, L"the start cell of the test maze has two open sides");
    }
};
REGISTER_TEST_CLASS(RestoredRunTests);