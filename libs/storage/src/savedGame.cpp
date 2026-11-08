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