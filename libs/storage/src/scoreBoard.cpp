#include "storage/scoreBoard.h"

#include "jsonFile.h"
#include "storage/dataFiles.h"
#include "storage/gameSettings.h"

#include <algorithm>
#include <ctime>

// Every field is checked while it is read back.
static ScoreEntry entryFromJson(const nlohmann::json& document)
{
    ScoreEntry entry;
    entry.playerName = cleanPlayerName(readField(document, "playerName", entry.playerName));
    entry.rowCount = std::max(0, readField(document, "rowCount", entry.rowCount));
    entry.columnCount = std::max(0, readField(document, "columnCount", entry.columnCount));
    entry.stepCount = std::max(0, readField(document, "stepCount", entry.stepCount));
    entry.keysUsed = std::max(0, readField(document, "keysUsed", entry.keysUsed));
    entry.score = std::max(0, readField(document, "score", entry.score));
    entry.elapsedSeconds = std::max(0.0, readField(document, "elapsedSeconds", entry.elapsedSeconds));
    entry.playedAt = readField(document, "playedAt", entry.playedAt);
    return entry;
}

static nlohmann::json entryToJson(const ScoreEntry& entry)
{
    return nlohmann::json{
        {"playerName", entry.playerName},
        {"rowCount", entry.rowCount},
        {"columnCount", entry.columnCount},
        {"stepCount", entry.stepCount},
        {"keysUsed", entry.keysUsed},
        {"score", entry.score},
        {"elapsedSeconds", entry.elapsedSeconds},
        {"playedAt", entry.playedAt} };
}

static void sortAndCut(std::vector<ScoreEntry>& entries)
{
    std::sort(entries.begin(), entries.end(),
        [](const ScoreEntry& first, const ScoreEntry& second) { return first.score > second.score; });

    if (entries.size() > static_cast<std::size_t>(maxScoreEntries))
    {
        entries.resize(static_cast<std::size_t>(maxScoreEntries));
    }
}

std::vector<ScoreEntry> loadScoreBoard()
{
    std::vector<ScoreEntry> entries;

    const std::optional<nlohmann::json> document = readJsonFile(scoreBoardFilePath());
    if (!document.has_value() || !document->is_object())
    {
        return entries;
    }

    if (!document->contains("entries") || !document->at("entries").is_array())
    {
        return entries;
    }

    for (const nlohmann::json& storedEntry : document->at("entries"))
    {
        if (storedEntry.is_object())
        {
            entries.push_back(entryFromJson(storedEntry));
        }
    }

    sortAndCut(entries);
    return entries;
}

bool saveScoreBoard(const std::vector<ScoreEntry>& entries)
{
    nlohmann::json storedList = nlohmann::json::array();
    for (const ScoreEntry& entry : entries)
    {
        storedList.push_back(entryToJson(entry));
    }

    const nlohmann::json document = { {"entries", storedList} };
    return writeJsonFile(scoreBoardFilePath(), document);
}

void addScoreEntry(std::vector<ScoreEntry>& entries, const ScoreEntry& newEntry)
{
    entries.push_back(newEntry);
    sortAndCut(entries);
}

int placeOfScore(const std::vector<ScoreEntry>& entries, int score)
{
    for (std::size_t entryIndex = 0; entryIndex < entries.size(); ++entryIndex)
    {
        if (entries[entryIndex].score == score)
        {
            return static_cast<int>(entryIndex) + 1;
        }
    }
    return 0;
}

std::string currentTimeText()
{
    const std::time_t now = std::time(nullptr);

    const std::tm* localTime = std::localtime(&now);
    if (localTime == nullptr)
    {
        return "unknown";
    }

    char textBuffer[32] = {};
    if (std::strftime(textBuffer, sizeof(textBuffer), "%Y-%m-%d %H:%M", localTime) == 0)
    {
        return "unknown";
    }
    return std::string(textBuffer);
}