#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>


std::optional<nlohmann::json> readJsonFile(const std::filesystem::path& filePath);

bool writeJsonFile(const std::filesystem::path& filePath, const nlohmann::json& document);


template <typename ValueType>
ValueType readField(const nlohmann::json& document, const char* fieldName, const ValueType& fallbackValue)
{
    if (!document.is_object() || !document.contains(fieldName))
    {
        return fallbackValue;
    }

    try
    {
        return document.at(fieldName).get<ValueType>();
    }
    catch (const nlohmann::json::exception&)
    {
        return fallbackValue;
    }
}
