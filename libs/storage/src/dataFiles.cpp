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
#elif defined(__APPLE__)
    const std::string homeFolder = environmentValue("HOME");
    if (!homeFolder.empty())
    {
        return std::filesystem::path(homeFolder) / "Library" / "Application Support";
    }
#else
    const std::string dataHome = environmentValue("XDG_DATA_HOME");
    if (!dataHome.empty())
    {
        return std::filesystem::path(dataHome);
    }

    const std::string homeFolder = environmentValue("HOME");
    if (!homeFolder.empty())
    {
        return std::filesystem::path(homeFolder) / ".local" / "share";
    }
#endif
    return std::filesystem::current_path(); // last resort
}

std::filesystem::path dataDirectory()
{
    std::filesystem::path directory = chosenDirectory;

    if (directory.empty())
    {
        const std::string folderFromEnvironment = environmentValue("LABYRINTH_DATA_DIR");
        directory = folderFromEnvironment.empty() ? userDataRoot() / folderName
            : std::filesystem::path(folderFromEnvironment);
    }

    std::error_code pathError;
    std::filesystem::create_directories(directory, pathError);
    return directory;
}

void useDataDirectory(const std::filesystem::path& directory)
{
    chosenDirectory = directory;
}

std::filesystem::path settingsFilePath()
{
    return dataDirectory() / "settings.json";
}

std::filesystem::path scoreBoardFilePath()
{
    return dataDirectory() / "scores.json";
}

std::filesystem::path savedGameFilePath()
{
    return dataDirectory() / "savedGame.json";
}
