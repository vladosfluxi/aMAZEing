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
std::string cleanPlayerName(const std::string& rawName)
{
    std::string cleanName;

    for (const char character : rawName)
    {
        const unsigned char plainCharacter = static_cast<unsigned char>(character);
        const bool isAllowed = std::isalnum(plainCharacter) != 0 || character == ' ' || character == '_' ||
            character == '-';
        if (isAllowed)
        {
            cleanName.push_back(character);
        }
    }

    
    const std::size_t firstUsefulCharacter = cleanName.find_first_not_of(' ');
    if (firstUsefulCharacter == std::string::npos)
    {
        return "Player";
    }

    const std::size_t lastUsefulCharacter = cleanName.find_last_not_of(' ');
    cleanName = cleanName.substr(firstUsefulCharacter, lastUsefulCharacter - firstUsefulCharacter + 1);

    if (cleanName.size() > static_cast<std::size_t>(maxPlayerNameLength))
    {
        cleanName.resize(static_cast<std::size_t>(maxPlayerNameLength));
    }

    return cleanName;
}
