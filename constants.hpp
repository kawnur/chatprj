#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <map>
#include <string>

const std::string logDelimiter { "############################" };

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
    DELETE,
    CLEAR_HISTORY
};

const std::map<CompanionActionType, std::string> actionTypeStringRepresentation
{
    { CompanionActionType::CREATE, std::string("Add new companion") },
    { CompanionActionType::UPDATE, std::string("Edit companion") }
};

const std::map<DialogType, std::string> dialogTypeStringRepresentation
{
    { DialogType::INFO, std::string("INFO") },
    { DialogType::WARNING, std::string("WARNING") },
    { DialogType::ERROR, std::string("ERROR") }
};

const std::string deleteCompanionDialogText { "Companion will be deleted with chat history." };
const std::string clearCompanionHistoryDialogText { "Companion chat history will be deleted." };

#endif // CONSTANTS_HPP
