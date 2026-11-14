// Tests for the JSON files of the game (the shared library labyrinthStorage).
#include "pch.h"

#include "core/gameSession.h"
#include "core/mazeGenerator.h"

#include "storage/dataFiles.h"
#include "storage/gameSettings.h"
#include "storage/savedGame.h"
#include "storage/scoreBoard.h"

#include <filesystem>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Gives the storage library an empty folder that belongs to this test alone.
static void useTestFolder(const char* testName)
{
    const std::filesystem::path testFolder =
        std::filesystem::temp_directory_path() / "labyrinthTests" / testName;

    std::error_code pathError;
    std::filesystem::remove_all(testFolder, pathError);

    useDataDirectory(testFolder);
}

// Writes a file by hand, the way a player with a text editor would.
static void writeTextFile(const std::filesystem::path& filePath, const std::string& content)
{
    std::ofstream outputFile(filePath, std::ios::trunc);
    outputFile << content;
}

// A short run that can be saved and read back.
static SavedGame makeSavedRun()
{
    GameSession session;
    session.startGame(generateMaze(12, 14, 2026u));
    session.movePlayer(Direction::east);
    session.movePlayer(Direction::south);
    session.addElapsedTime(42.5);

    SavedGame game;
    game.playerName = "Vlad";
    game.maze = session.maze();
    game.playerPosition = session.playerPosition();
    game.statistics = session.statistics();
    return game;
}

TEST_CLASS(SettingsTests)
{
public:
    TEST_METHOD(settingsAreWrittenAndReadBack)
    {
        useTestFolder("settingsAreWrittenAndReadBack");

        GameSettings settings;
        settings.playerName = "Vlad";
        settings.rowCount = 22;
        settings.columnCount = 33;
        settings.showHintPath = true;

        Assert::IsTrue(saveSettings(settings), L"the settings can be written");

        const GameSettings loadedSettings = loadSettings();
        Assert::AreEqual(std::string("Vlad"), loadedSettings.playerName, L"the name survives");
        Assert::AreEqual(22, loadedSettings.rowCount, L"the number of rows survives");
        Assert::AreEqual(33, loadedSettings.columnCount, L"the number of columns survives");
        Assert::IsTrue(loadedSettings.showHintPath, L"the hint setting survives");
    }

    TEST_METHOD(aNameIsCleanedBeforeItIsStored)
    {
        useTestFolder("aNameIsCleanedBeforeItIsStored");

        GameSettings settings;
        settings.playerName = "  Vlad!!<>  "; 
        Assert::IsTrue(saveSettings(settings), L"the settings can be written");

        Assert::AreEqual(std::string("Vlad"), loadSettings().playerName, L"the name is cleaned");
    }

    TEST_METHOD(aSizeOutOfTheRangeIsCorrected)
    {
        useTestFolder("aSizeOutOfTheRangeIsCorrected");

        GameSettings settings;
        settings.rowCount = 99; 
        settings.columnCount = 4; 
        Assert::IsTrue(saveSettings(settings), L"the settings can be written");

        const GameSettings loadedSettings = loadSettings();
        Assert::AreEqual(50, loadedSettings.rowCount, L"a size that is too big becomes 50");
        Assert::AreEqual(10, loadedSettings.columnCount, L"a size that is too small becomes 10");
    }

    TEST_METHOD(aDamagedSettingsFileGivesTheDefaults)
    {
        useTestFolder("aDamagedSettingsFileGivesTheDefaults");
        writeTextFile(settingsFilePath(), "{ this is not json ]");

        const GameSettings settings = loadSettings();
        Assert::AreEqual(std::string("Player"), settings.playerName, L"the default name is used");
        Assert::AreEqual(15, settings.rowCount, L"the default size is used");
    }

    TEST_METHOD(fieldsOfAWrongTypeAreIgnored)
    {
        useTestFolder("fieldsOfAWrongTypeAreIgnored");
        writeTextFile(settingsFilePath(), R"({"playerName": 42, "rowCount": "big"})");

        const GameSettings settings = loadSettings();
        Assert::AreEqual(std::string("Player"), settings.playerName, L"a field of a wrong type is ignored");
        Assert::AreEqual(15, settings.rowCount, L"a field of a wrong type keeps its default");
        Assert::AreEqual(15, settings.columnCount, L"a missing field keeps its default");
    }
};
REGISTER_TEST_CLASS(SettingsTests);

TEST_CLASS(ScoreBoardTests)
{
public:
    TEST_METHOD(onlyTheBestTenRunsAreKept)
    {
        std::vector<ScoreEntry> entries;
        for (int runIndex = 0; runIndex < 14; ++runIndex)
        {
            ScoreEntry entry;
            entry.playerName = "Player" + std::to_string(runIndex);
            entry.score = 100 * runIndex;
            addScoreEntry(entries, entry);
        }

        Assert::AreEqual(maxScoreEntries, static_cast<int>(entries.size()),
                         L"the list keeps only the best ten runs");
        Assert::AreEqual(1300, entries.front().score, L"the best run stands at the top");
        Assert::AreEqual(std::string("Player13"), entries.front().playerName, L"the best run keeps its name");
    }

    TEST_METHOD(thePlaceOfARunIsFound)
    {
        std::vector<ScoreEntry> entries;
        for (int runIndex = 0; runIndex < 3; ++runIndex)
        {
            ScoreEntry entry;
            entry.score = 100 * runIndex;
            addScoreEntry(entries, entry);
        }

        Assert::AreEqual(1, placeOfScore(entries, 200), L"the best run is on place one");
        Assert::AreEqual(3, placeOfScore(entries, 0), L"the weakest run is on the last place");
        Assert::AreEqual(0, placeOfScore(entries, 50), L"a run that is not in the list has no place");
    }

    TEST_METHOD(theScoreBoardIsWrittenAndReadBack)
    {
        useTestFolder("theScoreBoardIsWrittenAndReadBack");

        ScoreEntry entry;
        entry.playerName = "Vlad";
        entry.rowCount = 15;
        entry.columnCount = 15;
        entry.stepCount = 60;
        entry.keysUsed = 1;
        entry.score = 1500;
        entry.elapsedSeconds = 75.5;
        entry.playedAt = currentTimeText();

        std::vector<ScoreEntry> entries;
        addScoreEntry(entries, entry);
        Assert::IsTrue(saveScoreBoard(entries), L"the score board can be written");

        const std::vector<ScoreEntry> loadedEntries = loadScoreBoard();
        Assert::AreEqual(1, static_cast<int>(loadedEntries.size()), L"the run is read back");
        Assert::AreEqual(std::string("Vlad"), loadedEntries.front().playerName, L"the name survives");
        Assert::AreEqual(1500, loadedEntries.front().score, L"the points survive");
        Assert::AreEqual(75.5, loadedEntries.front().elapsedSeconds, 0.001, L"the time survives");
    }

    TEST_METHOD(aScoreFileOfTheWrongShapeGivesAnEmptyList)
    {
        useTestFolder("aScoreFileOfTheWrongShapeGivesAnEmptyList");
        writeTextFile(scoreBoardFilePath(), "[1, 2, 3]");

        Assert::IsTrue(loadScoreBoard().empty(), L"a file of the wrong shape gives an empty list");
    }
};
REGISTER_TEST_CLASS(ScoreBoardTests);

TEST_CLASS(SavedGameTests)
{
public:
    TEST_METHOD(aRunIsWrittenAndReadBack)
    {
        useTestFolder("aRunIsWrittenAndReadBack");
        const SavedGame game = makeSavedRun();

        Assert::IsTrue(saveGame(game), L"a run can be written");
        Assert::IsTrue(hasSavedGame(), L"the game sees that a save file exists");

        const std::optional<SavedGame> restored = loadGame();
        Assert::IsTrue(restored.has_value(), L"the run can be read back");
        Assert::AreEqual(12, restored->maze.rowCount(), L"the saved maze keeps its rows");
        Assert::AreEqual(14, restored->maze.columnCount(), L"the saved maze keeps its columns");
        Assert::IsTrue(restored->playerPosition == game.playerPosition, L"the player stands where it stood");
        Assert::AreEqual(game.statistics.stepCount, restored->statistics.stepCount,
                         L"the step count survives");
        Assert::AreEqual(42.5, restored->statistics.elapsedSeconds, 0.001, L"the played time survives");
    }

    TEST_METHOD(everySingleCellSurvivesTheRoundTrip)
    {
        useTestFolder("everySingleCellSurvivesTheRoundTrip");
        const SavedGame game = makeSavedRun();
        Assert::IsTrue(saveGame(game), L"a run can be written");

        const std::optional<SavedGame> restored = loadGame();
        Assert::IsTrue(restored.has_value(), L"the run can be read back");

        for (int cellIndex = 0; cellIndex < game.maze.cellCount(); ++cellIndex)
        {
            const Cell& written = game.maze.cellAtIndex(cellIndex);
            const Cell& read = restored->maze.cellAtIndex(cellIndex);

            Assert::IsTrue(written.walls == read.walls, L"the walls of the cell survive");
            Assert::AreEqual(written.stepCost, read.stepCost, L"the number of the cell survives");
            Assert::IsTrue(written.hasKey == read.hasKey, L"the key of the cell survives");
            Assert::IsTrue(written.visitedByPlayer == read.visitedByPlayer, L"the cell keeps its trail");
        }
    }

    TEST_METHOD(aHalfWrittenSaveFileIsRefused)
    {
        useTestFolder("aHalfWrittenSaveFileIsRefused");
        writeTextFile(savedGameFilePath(), R"({"rowCount": 12, "columnCount": 14, "cells": [)");

        Assert::IsFalse(loadGame().has_value(), L"a half written save file is refused");
    }

    TEST_METHOD(aSaveFileWithTooFewCellsIsRefused)
    {
        useTestFolder("aSaveFileWithTooFewCellsIsRefused");
        writeTextFile(savedGameFilePath(),
                      R"({"rowCount": 12, "columnCount": 14, "cells": [{"walls": "1111"}]})");

        Assert::IsFalse(loadGame().has_value(), L"the maze and the cell list have to fit together");
    }

    TEST_METHOD(aSaveFileWithAnIllegalSizeIsRefused)
    {
        useTestFolder("aSaveFileWithAnIllegalSizeIsRefused");
        writeTextFile(savedGameFilePath(), R"({"rowCount": 2, "columnCount": 2, "cells": []})");

        Assert::IsFalse(loadGame().has_value(), L"a maze of 2x2 is not allowed");
    }

    TEST_METHOD(theSaveFileCanBeDeleted)
    {
        useTestFolder("theSaveFileCanBeDeleted");
        Assert::IsTrue(saveGame(makeSavedRun()), L"a run can be written");

        Assert::IsTrue(removeSavedGame(), L"the save file can be deleted");
        Assert::IsFalse(hasSavedGame(), L"after deleting there is no save file any more");
        Assert::IsFalse(loadGame().has_value(), L"a missing save file is simply empty");
    }
};
REGISTER_TEST_CLASS(SavedGameTests);
