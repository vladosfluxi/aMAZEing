#include "jsonFile.h"

#include <fstream>

std::optional<nlohmann::json> readJsonFile(const std::filesystem::path& filePath)
{
    std::error_code pathError;
    if (!std::filesystem::exists(filePath, pathError) || pathError)
    {
        return std::nullopt;
    }

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open())
    {
        return std::nullopt;
    }

    try
    {
        return nlohmann::json::parse(inputFile);
    }
    catch (const nlohmann::json::parse_error&)
    {
        return std::nullopt; // the file exists but its content is broken
    }
}

bool writeJsonFile(const std::filesystem::path& filePath, const nlohmann::json& document)
{
    std::error_code pathError;
    std::filesystem::create_directories(filePath.parent_path(), pathError);

    std::ofstream outputFile(filePath, std::ios::trunc);
    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile << document.dump(2) << '\n';
    return outputFile.good();
}
