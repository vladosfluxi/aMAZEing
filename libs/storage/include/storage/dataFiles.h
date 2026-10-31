#pragma once

#include "storage/storageApi.h"

#include <filesystem>

storageApi std::filesystem::path dataDirectory();

// Used by the tests so they never touch the real files of the player.
storageApi void useDataDirectory(const std::filesystem::path& directory);

storageApi std::filesystem::path settingsFilePath();
storageApi std::filesystem::path scoreBoardFilePath();
storageApi std::filesystem::path savedGameFilePath();
