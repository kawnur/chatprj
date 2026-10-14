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
// #include "db_constants.hpp"
// #include "db_interaction.hpp"
#include "db_operator.hpp"
#include "file_manager.hpp"
#include "message_handler.hpp"
#include "network_manager.hpp"
#include "utils.hpp"

class Action;
class ActionResult;

template<typename T>
class ActionSharedValueResult;

class Companion;
class CompanionAction;
// class DBOperator;
class Message;
class MessageData;
class MessageInfo;
class MessageHandler;
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

// class Manager : public QObject // TODO do we need inheritance?
class Manager : public QObject, public std::enable_shared_from_this<Manager>
{
public:
    Manager();
    ~Manager();

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(DBRequestType type, Ts &&...args)
    {
        return dbOperator_.getDBData(type, args...);
    }

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
    CompanionPtr getMappedCompanionById(int id) const;
    CompanionPtr getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget *widget) const;
    std::shared_ptr<WidgetGroup> getMappedWidgetGroupByCompanion(CompanionPtr companion) const;

    bool pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data);

    // void receiveMessage(CompanionPtr companion, const std::string &json);
    void receiveMessage(int companionId, const std::string &json);

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
    void updateSocketInfo(int id, std::shared_ptr<CompanionData> data);
    ActionResultPtr deleteCompanion(CompanionActionPtr action);
    void clearChatHistory(CompanionPtr companion);
    ActionResultPtr clearCompanionHistory(CompanionActionPtr action);
    ActionResultPtr pushPasswordToDbAndReturnId(PasswordActionPtr action);
    ActionResultPtr createUserPassword(PasswordActionPtr action);
    ActionResultPtr authenticateUser(PasswordActionPtr action);
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void startUserAuthentication();
    void requestHistoryFromCompanion(CompanionPtr companion);
    void sendChatHistoryToCompanion(CompanionPtr companion);
    // bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);
    void endAction(ActionPtr action);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendMessage(
        CompanionPtr companion, std::shared_ptr<Message> message,
        std::shared_ptr<MessageMetaData> meta);

    MessageMetaDataPtr pushMessageToDB(
        MessageMetaDataPtr meta, MessageDataPtr data, MessageStatePtr state);

    void markMessageAsSent(CompanionPtr companion, MessagePtr message);

    // void markMessageAsReceived(
    //     CompanionPtr companion, MessagePtr message);
    void markMessageAsReceived(std::shared_ptr<MessageInfo> info);

    // void waitForMessageReceptionConfirmation(CompanionPtr companion, MessagePtr message);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    void addEarlyMessages(CompanionPtr companion);
    void sendUnsentMessages(CompanionPtr companion);
    std::shared_ptr<NetworkEntity> getNetworkEntity(int id);

    bool sendFileBlock(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    std::shared_ptr<CompanionData> getCompanionData(CompanionPtr companion);
    // std::string getIpAddress(CompanionPtr companion);
    // uint16_t getServerPort(CompanionPtr companion);
    // uint16_t getClientPort(CompanionPtr companion);

    bool connectClient(CompanionPtr companion);
    bool disconnectClient(CompanionPtr companion);

    void addSenderOperator(const std::string &networkId, const std::filesystem::path &path);

    void addReceiverOperator(
        std::shared_ptr<MessageMetaData> meta, const std::filesystem::path &path);

    template<typename T>
    std::shared_ptr<T> getFileOperator(const std::string &networkId)
    {
        return fileManager_.getFileOperator<T>(networkId);
    }

    template<typename T>
    void removeFileOperator(const std::string &networkId)
    {
        fileManager_.removeFileOperator<T>(networkId);
    }

    std::string getFilePathString(const std::string &networkId);
    bool setFileOperatorFilePath(const std::string &networkId, const std::filesystem::path &path);
    std::string getFileOperatorMD5Hash(const std::string &networkId);

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
    bool buildCompanions();
    void buildWidgetGroups();
    CompanionPtr addCompanionObject(int id, const std::string &name);
    std::shared_ptr<CompanionResult> addCompanionToManagerMap(int id, const std::string &name);

    std::shared_ptr<ActionResult> addCompanionToNetworkManagerMap(
        int id, std::shared_ptr<CompanionData> data);

    void createWidgetGroupAndAddToMapping(CompanionPtr companion);
    void deleteCompanionObject(CompanionPtr companion);
    void deleteWidgetGroupAndDeleteFromMapping(CompanionPtr companion);

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

    DBOperator dbOperator_;
    MessageHandler messageHandler_;
    FileManager fileManager_;
    std::mutex messageStateToMessageMapMutex_;
    NetworkManager networkManager_;
    bool userIsAuthenticated_;
    CompanionPtr selectedCompanion_;

    std::map<int, std::pair<CompanionPtr, std::shared_ptr<WidgetGroup>>>
        companionAndWidgetGroupMap_;

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
