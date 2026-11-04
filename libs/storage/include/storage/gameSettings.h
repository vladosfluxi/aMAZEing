#pragma once

#include "storage/storageApi.h"

#include <string>

inline constexpr int maxPlayerNameLength = 16;

struct GameSettings
{
    std::string playerName = "Player";
    int rowCount = 15;
    int columnCount = 15;
    bool showHintPath = false;
};


storageApi GameSettings loadSettings();

storageApi bool saveSettings(const GameSettings& settings);
storageApi std::string cleanPlayerName(const std::string& rawName);
