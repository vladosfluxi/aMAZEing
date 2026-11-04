#pragma once

#include "storage/storageApi.h"

#include "core/gameSession.h"
#include "core/mazeGrid.h"

#include <optional>
#include <string>

struct SavedGame
{
    std::string playerName = "Player";
    MazeGrid maze;
    Position playerPosition;
    GameStatistics statistics;
};

storageApi bool saveGame(const SavedGame& game);

// empty when there is no save file, or when it cannot be trusted.
storageApi std::optional<SavedGame> loadGame();

storageApi bool hasSavedGame();
storageApi bool removeSavedGame();
