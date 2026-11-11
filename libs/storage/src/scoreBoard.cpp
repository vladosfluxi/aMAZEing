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