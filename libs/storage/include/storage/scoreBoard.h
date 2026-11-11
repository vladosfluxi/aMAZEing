#pragma once

#include "storage/storageApi.h"

#include <string>
#include <vector>

inline constexpr int maxScoreEntries = 10;

struct ScoreEntry
{
    std::string playerName = "Player";
    int rowCount = 0;
    int columnCount = 0;
    int stepCount = 0;
    int keysUsed = 0;
    int score = 0;
    double elapsedSeconds = 0.0;
    std::string playedAt; 
};

storageApi std::vector<ScoreEntry> loadScoreBoard();
storageApi bool saveScoreBoard(const std::vector<ScoreEntry>& entries);

storageApi void addScoreEntry(std::vector<ScoreEntry>& entries, const ScoreEntry& newEntry);

storageApi int placeOfScore(const std::vector<ScoreEntry>& entries, int score);

storageApi std::string currentTimeText();
