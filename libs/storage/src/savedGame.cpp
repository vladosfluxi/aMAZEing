#include "storage/savedGame.h"

#include "jsonFile.h"
#include "storage/dataFiles.h"
#include "storage/gameSettings.h"

#include "core/mazeTypes.h"

#include <algorithm>
#include <string>

static constexpr int saveFormatVersion = 1;

// Anything unknown becomes a wall, which is the safe direction.
static WallState wallFromNumber(int number)
{
    switch (number)
    {
    case 0:
        return WallState::open;
    case 2:
        return WallState::door;
    default:
        return WallState::solid;
    }
}

static std::string wallsToText(const Cell& cell)
{
    std::string wallText;
    for (const Direction direction : allDirections)
    {
        const WallState wall = cell.walls[static_cast<std::size_t>(indexOfDirection(direction))];
        wallText += static_cast<char>('0' + static_cast<int>(wall));
    }
    return wallText;
}

static void wallsFromText(Cell& cell, const std::string& wallText)
{
    if (wallText.size() != static_cast<std::size_t>(directionCount))
    {
        return; // a damaged cell stays closed on every side
    }

    for (std::size_t wallIndex = 0; wallIndex < wallText.size(); ++wallIndex)
    {
        cell.walls[wallIndex] = wallFromNumber(wallText[wallIndex] - '0');
    }
}

static nlohmann::json cellToJson(const Cell& cell)
{
    return nlohmann::json{
        {"walls", wallsToText(cell)},
        {"hasKey", cell.hasKey},
        {"visitedByPlayer", cell.visitedByPlayer},
        {"stepCost", cell.stepCost} };
}

static Cell cellFromJson(const nlohmann::json& document)
{
    Cell cell;
    cell.hasKey = readField(document, "hasKey", false);
    cell.visitedByPlayer = readField(document, "visitedByPlayer", false);
    cell.stepCost = std::clamp(readField(document, "stepCost", 1), 0, 9);
    wallsFromText(cell, readField(document, "walls", std::string()));
    return cell;
}

bool saveGame(const SavedGame& game)
{
    if (game.maze.isEmpty())
    {
        return false;
    }

    nlohmann::json cellList = nlohmann::json::array();
    for (int cellIndex = 0; cellIndex < game.maze.cellCount(); ++cellIndex)
    {
        cellList.push_back(cellToJson(game.maze.cellAtIndex(cellIndex)));
    }

    const nlohmann::json document = {
        {"version", saveFormatVersion},
        {"playerName", cleanPlayerName(game.playerName)},
        {"rowCount", game.maze.rowCount()},
        {"columnCount", game.maze.columnCount()},
        {"playerRow", game.playerPosition.row},
        {"playerColumn", game.playerPosition.column},
        {"statistics",
         {{"stepCount", game.statistics.stepCount},
          {"keyCount", game.statistics.keyCount},
          {"keysUsed", game.statistics.keysUsed},
          {"collectedCost", game.statistics.collectedCost},
          {"elapsedSeconds", game.statistics.elapsedSeconds}}},
        {"cells", cellList} };

    return writeJsonFile(savedGameFilePath(), document);
}

std::optional<SavedGame> loadGame()
{
    const std::optional<nlohmann::json> document = readJsonFile(savedGameFilePath());
    if (!document.has_value() || !document->is_object())
    {
        return std::nullopt;
    }

    const int rowCount = readField(*document, "rowCount", 0);
    const int columnCount = readField(*document, "columnCount", 0);
    if (!isValidMazeSide(rowCount) || !isValidMazeSide(columnCount))
    {
        return std::nullopt; // not a legal maze size
    }

    if (!document->contains("cells") || !document->at("cells").is_array())
    {
        return std::nullopt;
    }

    const nlohmann::json& cellList = document->at("cells");
    if (cellList.size() != static_cast<std::size_t>(rowCount * columnCount))
    {
        return std::nullopt; // the size and the cell list do not fit together
    }

    SavedGame game;
    game.playerName = cleanPlayerName(readField(*document, "playerName", std::string("Player")));
    game.maze = MazeGrid(rowCount, columnCount);
    for (std::size_t cellIndex = 0; cellIndex < cellList.size(); ++cellIndex)
    {
        if (!cellList[cellIndex].is_object())
        {
            return std::nullopt;
        }
        game.maze.cellAtIndex(static_cast<int>(cellIndex)) = cellFromJson(cellList[cellIndex]);
    }

    game.playerPosition.row = std::clamp(readField(*document, "playerRow", 0), 0, rowCount - 1);
    game.playerPosition.column = std::clamp(readField(*document, "playerColumn", 0), 0, columnCount - 1);

    if (document->contains("statistics") && document->at("statistics").is_object())
    {
        const nlohmann::json& storedStatistics = document->at("statistics");
        game.statistics.stepCount = std::max(0, readField(storedStatistics, "stepCount", 0));
        game.statistics.keyCount = std::max(0, readField(storedStatistics, "keyCount", 0));
        game.statistics.keysUsed = std::max(0, readField(storedStatistics, "keysUsed", 0));
        game.statistics.collectedCost = std::max(0, readField(storedStatistics, "collectedCost", 0));
        game.statistics.elapsedSeconds = std::max(0.0, readField(storedStatistics, "elapsedSeconds", 0.0));
    }

    return game;
}

bool hasSavedGame()
{
    std::error_code pathError;
    return std::filesystem::exists(savedGameFilePath(), pathError) && !pathError;
}

bool removeSavedGame()
{
    std::error_code pathError;
    return std::filesystem::remove(savedGameFilePath(), pathError) && !pathError;
}
