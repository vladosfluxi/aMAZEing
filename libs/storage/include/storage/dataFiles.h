#pragma once

#include "storage/storageApi.h"

#include <filesystem>

// The folder with the files of the game, created when it is missing.
// Tried in this order: what useDataDirectory() was given, the environment
// variable LABYRINTH_DATA_DIR, and then the usual place of the system
// (%APPDATA%, ~/Library/Application Support, ~/.local/share).
storageApi std::filesystem::path dataDirectory();

// Used by the tests so they never touch the real files of the player.
storageApi void useDataDirectory(const std::filesystem::path& directory);

storageApi std::filesystem::path settingsFilePath();
storageApi std::filesystem::path scoreBoardFilePath();
storageApi std::filesystem::path savedGameFilePath();
