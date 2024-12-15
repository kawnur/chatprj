#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <string>

const std::string LOG_DELIMITER { "############################" };

enum class DialogType
{
    INFO,
    WARNING,
    ERROR
};

enum class CompanionActionType
{
    CREATE,
    // READ,
    UPDATE,
    DELETE
};

const std::map<DialogType, std::string> dialogTypeStringRepresentation
{
    { DialogType::INFO, std::string("INFO") },
    { DialogType::WARNING, std::string("WARNING") },
    { DialogType::ERROR, std::string("ERROR") }
};

#endif // CONSTANTS_HPP
