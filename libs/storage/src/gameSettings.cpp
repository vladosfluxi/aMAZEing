#include "storage/gameSettings.h"

#include "jsonFile.h"
#include "storage/dataFiles.h"

#include "core/mazeTypes.h"

#include <cctype>

GameSettings loadSettings()
{
    GameSettings settings;

    const std::optional<nlohmann::json> document = readJsonFile(settingsFilePath());
    if (!document.has_value())
    {
        return settings; 
    }

    settings.playerName = cleanPlayerName(readField(*document, "playerName", settings.playerName));
    settings.rowCount = clampMazeSide(readField(*document, "rowCount", settings.rowCount));
    settings.columnCount = clampMazeSide(readField(*document, "columnCount", settings.columnCount));
    settings.showHintPath = readField(*document, "showHintPath", settings.showHintPath);
    return settings;
}

bool saveSettings(const GameSettings& settings)
{
    const nlohmann::json document = {
        {"playerName", cleanPlayerName(settings.playerName)},
        {"rowCount", clampMazeSide(settings.rowCount)},
        {"columnCount", clampMazeSide(settings.columnCount)},
        {"showHintPath", settings.showHintPath} };

    return writeJsonFile(settingsFilePath(), document);
}