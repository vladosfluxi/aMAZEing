#include "storage/dataFiles.h"

#include <cstdlib>
#include <string>

static constexpr const char* folderName = "labyrinth";


static std::filesystem::path chosenDirectory;

static std::string environmentValue(const char* variableName)
{
    const char* value = std::getenv(variableName);
    return value != nullptr ? std::string(value) : std::string();
}

static std::filesystem::path userDataRoot()
{
#if defined(_WIN32)
    const std::string applicationData = environmentValue("APPDATA");
    if (!applicationData.empty())
    {
        return std::filesystem::path(applicationData);
    }

