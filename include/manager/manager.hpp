#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include <libpq-fe.h>

#include <QString>
#include <QWidget>

#include "action.hpp"
#include "action_result.hpp"
#include "constants.hpp"
#include "db_constants.hpp"
#include "db_interaction.hpp"
#include "utils.hpp"

class Action;
class ActionResult;

template<typename T>
class ActionSharedValueResult;

class Companion;
class CompanionAction;
class DBReplyData;
class DBRequester;
class Message;
class MessageData;
class MessageInfo;
class MessageMetaData;
class MessageState;
class PasswordAction;
class SocketInfoBaseWidget;
class WidgetGroup;

using CompanionPtr = std::shared_ptr<Companion>;
using CompanionResult = ActionSharedValueResult<Companion>;
using ActionResultPtr = std::shared_ptr<ActionResult>;
using ActionPtr = std::shared_ptr<Action>;
using CompanionActionPtr = std::shared_ptr<CompanionAction>;
using PasswordActionPtr = std::shared_ptr<PasswordAction>;
using FileActionPtr = std::shared_ptr<FileAction>;
using MessagePtr = std::shared_ptr<Message>;
using MessageMetaDataPtr = std::shared_ptr<MessageMetaData>;
using MessageDataPtr = std::shared_ptr<MessageData>;
using MessageStatePtr = std::shared_ptr<MessageState>;

int getDataFromDBResult(
    bool log, std::shared_ptr<DBReplyData> data, std::shared_ptr<PGresult> result, int maxTuples);

template <typename T, typename...Ts>
std::shared_ptr<T> buildObjectFromJson(const nlohmann::json &data, Ts &&...args)
{
    auto object = std::make_shared<T>();
    auto result = object->setFields(data, args...);

    if (!result)
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

    return (result) ? object : nullptr;
}

template <typename T, typename...Ts>
bool updateObjectFromJson(std::shared_ptr<T> object, const nlohmann::json &data, Ts &&...args)
{
    auto result = object->setFields(data, args...);

    if (!result)
        logTemplateError("{}, error parsing jsonData", __FUNCTION__);

    return result;
}

class Manager : public QObject // TODO do we need inheritance?
{
public:
    Manager();
    ~Manager();

    template<typename F, typename T>
    ActionResultPtr performActionAndCallPostAct(F &&func, std::shared_ptr<T> action)
    {
        auto result = func(action);
        action->postAct(result);

        return result;
    }

    CompanionPtr getSelectedCompanion();
    bool userIsAuthenticated();
    void set();

    // CompanionPtr getMappedCompanionBySocketInfoBaseWidget(std::shared_ptr<SocketInfoBaseWidget>) const;
    CompanionPtr getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget *widget) const;
    std::shared_ptr<WidgetGroup> getMappedWidgetGroupByCompanion(CompanionPtr companion) const;
    NetworkMessageType defineNetworkMessageType(MessageType type);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    void sendFile(CompanionPtr companion, const std::filesystem::path &path);

    void receiveTextMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveFileProposalMessage(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmation(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void receiveConfirmationRequest(
        CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void reciveChatHistoryRequest(CompanionPtr companion);
    void receiveChatHistoryData(CompanionPtr companion, const nlohmann::json &data);
    void receiveFileRequest(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileData(CompanionPtr companion, MessageMetaDataPtr meta, MessageDataPtr data);
    void receiveFileDataCheck(CompanionPtr companion, MessageMetaDataPtr meta, bool success);
    void receiveFileDataTransmissionEnd(CompanionPtr companion, MessageMetaDataPtr meta);
    void receiveFileDataTransmissionFailure(CompanionPtr companion, MessageMetaDataPtr meta);
    MessageDataPtr buildMessageDataFromJson(const nlohmann::json &jsonData);
    MessageMetaDataPtr buildMessageMetaDataFromJson(const nlohmann::json &jsonData);
    MessageStatePtr buildMessageStateFromJson(const nlohmann::json &jsonData);
    bool pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data);
    void receiveMessage(CompanionPtr companion, const std::string &json);
    void addEarlyMessages(CompanionPtr companion);
    void resetSelectedCompanion(CompanionPtr companion);

    template<typename T>
    std::function<ActionResultPtr(std::shared_ptr<T>)> getActionLambda(std::shared_ptr<T> action);

    template<typename T>
    ActionResultPtr performAction(std::shared_ptr<T> action)
    {
        auto lambda = getActionLambda<T>(action);

        return performActionAndCallPostAct(lambda, action);
    }

    template<typename T, typename... Ts>
    void initAction(ActionType type, Ts &&...args)
    {
        auto action = std::make_shared<T>(type, args...);
        setCurrentAction(std::dynamic_pointer_cast<Action>(action));  // TODO check cast
        action->set();
    }

    void initCompanionCreation();
    void initCompanionUpdate(CompanionPtr companion);
    void initCompanionDeletion(CompanionPtr companion);
    void initCompanionHistoryClearing(CompanionPtr companion);
    void initEntrancePasswordCreation();
    void initEntrancePasswordReception();
    void initFileSend(std::shared_ptr<Companion> companion);
    void initFileReception(const std::string &networkId, std::shared_ptr<Companion> companion);

    ActionResultPtr createCompanion(CompanionActionPtr action);

    // void updateCompanion(CompanionActionPtr action);

    ActionResultPtr updateCompanion(CompanionActionPtr action);
    ActionResultPtr deleteCompanion(CompanionActionPtr action);
    void clearChatHistory(CompanionPtr companion);
    ActionResultPtr clearCompanionHistory(CompanionActionPtr action);
    ActionResultPtr pushPasswordToDbAndReturnId(PasswordActionPtr action);
    ActionResultPtr createUserPassword(PasswordActionPtr action);
    ActionResultPtr authenticateUser(PasswordActionPtr action);
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void startUserAuthentication();
    void sendUnsentMessages(CompanionPtr companion);
    void requestHistoryFromCompanion(CompanionPtr companion);
    void sendChatHistoryToCompanion(CompanionPtr companion);
    // bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);
    void endAction(ActionPtr action);

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(DBRequestType type, Ts &&...args)
    {
        DBRequestData requestData { type };
        auto data = dbRequester_.getDBData(requestData, args...);

        if (!data) {
            auto entry = getStringByFormat("{0}, {1}", requestData.getLogMark(), DB_REPLY_NULL);
            // showErrorDialogAndLogError(entry);

            return nullptr;
        }

        if (data->isEmpty()) {
            auto entry = getStringByFormat("{0}, {1}", requestData.getLogMark(), DB_REPLY_EMPTY);
            // showWarningDialogAndLogWarning(entry);

            // return nullptr;
        }

        return data;
    }

private:
    ActionResultPtr getActionResultByKeyDBData(
        std::shared_ptr<DBReplyData> data, const std::string &key, bool allowEmptyResult = false);

    ActionResultPtr pushCompanionToDbAndReturnId(CompanionActionPtr action);
    uint16_t getServerPortByCompanionId(int id);
    ActionResultPtr pushSocketToDb(CompanionActionPtr action, int serverPort);
    ActionResultPtr updateCompanionInDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionMessagesFromDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionAndSocketFromDbAndReturnId(CompanionActionPtr action);
    CompanionPtr getMappedCompanionByWidgetGroup(std::shared_ptr<WidgetGroup> group) const;
    void fillCompanionMessageMapping(CompanionPtr companion, bool containersNotEmpty);
    bool buildCompanions();
    void buildWidgetGroups();
    CompanionPtr addCompanionObject(int id, const std::string &name);
    std::shared_ptr<CompanionResult> getCompanionAdditionResult(int id, const std::string &name);
    void createWidgetGroupAndAddToMapping(CompanionPtr companion);
    void deleteCompanionObject(CompanionPtr companion);
    void deleteWidgetGroupAndDeleteFromMapping(CompanionPtr companion);

    ActionResultPtr checkCompanionDataForExistanceAtCreation(CompanionActionPtr action);
    ActionResultPtr checkCompanionDataForExistanceAtUpdate(CompanionActionPtr action);
    ActionResultPtr checkPasswordForExistanceAtAuthentication(PasswordActionPtr action);
    void waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message);
    void markMessageAsSent(CompanionPtr companion, MessagePtr message);

    // void markMessageAsReceived(
    //     CompanionPtr companion, MessagePtr message);
    void markMessageAsReceived(std::shared_ptr<MessageInfo> info);

    MessageMetaDataPtr pushMessageToDB(
        MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    template<typename... Ts>
    ActionResultPtr getActionResult(Ts &&...args)
    {
        auto data = getDBData(args...);

        if (!data)
            return std::make_shared<ActionResult>(false, DB_REPLY_NULL);

        if (data->isEmpty())
            return std::make_shared<ActionResult>(false, DB_REPLY_EMPTY);

        return std::make_shared<ActionResult>(true, ""s);
    }

    void setCurrentAction(ActionPtr action);
    void checkAndResetCurrentAction(ActionPtr action);

    // bool initialized_;
    DBRequester dbRequester_;
    std::mutex messageStateToMessageMapMutex_;
    std::shared_ptr<PGconn> dbConnection_;
    bool userIsAuthenticated_;
    CompanionPtr selectedCompanion_;

    std::map<int, std::pair<CompanionPtr, std::shared_ptr<WidgetGroup>>>
        mapCompanionToWidgetGroup_;

    std::filesystem::path lastOpenedPath_;
    ActionPtr currentAction_;
};

template<>
std::function<ActionResultPtr(CompanionActionPtr)>
Manager::getActionLambda(CompanionActionPtr action);

template<>
std::function<ActionResultPtr(PasswordActionPtr)>
Manager::getActionLambda(PasswordActionPtr action);

template<>
std::function<ActionResultPtr(FileActionPtr)>
Manager::getActionLambda(FileActionPtr action);

std::shared_ptr<Manager> getManager();

#endif // MANAGER_HPP
