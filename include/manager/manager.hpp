#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <libpq-fe.h>

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"
#include "db_constants.hpp"
#include "db_operator.hpp"
#include "file_manager.hpp"
#include "message_handler.hpp"
#include "network_manager.hpp"

// TODO do we need QObject inheritance?
class Manager : public QObject, public std::enable_shared_from_this<Manager>
{
public:
    Manager(GraphicManagerPtr graphicManager);
    ~Manager();

    // bool isInitialised();
    // bool userIsAuthenticated();
    void set();

    // db
    template<typename... Ts>
    DBReplyDataPtr getDBData(DBRequestType type, Ts &&...args)
    {
        return dbOperator_.getDBData(type, args...);
    }

    MessagePtr pushMessageToDB(MessagePtr message);
    bool pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data);
    DBReplyDataPtr getEarlyMessagesDBData(int companionId, int messageId);

    // action
    template<typename F, typename T>
    ActionResultPtr performActionAndCallPostAct(F &&func, std::shared_ptr<T> action)
    {
        auto result = func(action);
        action->postAct(result);

        return result;
    }

    template<typename T>
    std::function<ActionResultPtr(std::shared_ptr<T>)> getActionLambda(std::shared_ptr<T> action);

    template<typename T>
    ActionResultPtr performAction(std::shared_ptr<T> action)
    {
        auto lambda = getActionLambda<T>(action);

        return performActionAndCallPostAct(lambda, action);
    }

    void initCompanionCreation();
    void initCompanionUpdate(CompanionPtr companion);
    void initCompanionDeletion(CompanionPtr companion);
    void initCompanionHistoryClearing(CompanionPtr companion);
    void initEntrancePasswordCreation();
    void initEntrancePasswordReception();
    void initFileSend(CompanionPtr companion);
    void initFileReception(const std::string &networkId, CompanionPtr companion);
    void startUserAuthentication();
    void endAction(ActionPtr action);

    // message
    void receiveMessage(int companionId, const std::string &json);
    void requestHistoryFromCompanion(CompanionPtr companion);
    void sendHistoryToCompanion(CompanionPtr companion);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendFileRequestMessage(CompanionPtr companion, const std::string &networkId);

    bool sendFileDataTransmissionResultMessage(
        CompanionPtr companion, NetworkMessageType type, const std::string &networkId);

    void addEarlyMessages(CompanionPtr companion);

    bool setMappedMessageWidget(
        CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget);

    void fillCompanionMessageMapping(CompanionPtr companion, bool containersAreEmpty);

    // widgets
    void clearChatHistory(CompanionPtr companion);
    void markMessageAsSent(CompanionPtr companion, MessagePtr message);
    void markMessageAsReceived(MessageInfoPtr info);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    void addMessageWidgets(CompanionPtr companion, const std::vector<MessagePtr> &messages);
    void disableBlur();

    // files
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);
    void addSenderOperator(const std::string &networkId, ActionPtr action);
    void addReceiverOperator(MessagePtr message);
    void removeSenderOperator(const std::string &networkId);
    std::string getFilePathString(const std::string &networkId);
    bool setFileOperatorFilePath(const std::string &networkId, const std::filesystem::path &path);
    std::string getFileOperatorMD5Hash(const std::string &networkId);

    bool sendFileBlock(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    void sendFile(CompanionPtr companion, MessagePtr message);
    void receiveFileData(CompanionPtr companion, MessagePtr message);
    void receiveFile(CompanionPtr companion, MessagePtr message);
    std::optional<bool> stopFileDataReceiving(MessagePtr message);
    void stopFileDataSending(MessagePtr message);

    // network
    CompanionDataPtr getCompanionData(CompanionPtr companion);
    bool connectClient(CompanionPtr companion);
    bool disconnectClient(CompanionPtr companion);
    bool clientAction(CompanionPtr companion, bool isConnected);
    bool sendJsonByNetwork(CompanionPtr companion, const std::string &json);

private:
    bool buildCompanions();
    CompanionPtr addCompanion(int id, const std::string &name);
    CompanionPtr addCompanion(int id, const std::string &name, std::string &entry);

    // db
    ActionResultPtr pushSocketToDb(CompanionActionPtr action, int serverPort);
    ActionResultPtr updateCompanionInDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionMessagesFromDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionAndSocketFromDbAndReturnId(CompanionActionPtr action);

    // action
    template<typename T, typename... Ts>
    void initAction(ActionType type, Ts &&...args)
    {
        auto action = std::make_shared<T>(type, args...);
        setCurrentAction(std::dynamic_pointer_cast<Action>(action));  // TODO check cast
        action->set();
    }

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

    ActionResultPtr getActionResultByKeyDBData(
        DBReplyDataPtr data, const std::string &key, bool allowEmptyResult = false);

    ActionResultPtr createCompanion(CompanionActionPtr action);
    ActionResultPtr updateCompanion(CompanionActionPtr action);
    ActionResultPtr deleteCompanion(CompanionActionPtr action);
    ActionResultPtr clearCompanionHistory(CompanionActionPtr action);
    ActionResultPtr pushPasswordToDbAndReturnId(PasswordActionPtr action);
    ActionResultPtr createUserPassword(PasswordActionPtr action);
    ActionResultPtr authenticateUser(PasswordActionPtr action);
    ActionResultPtr pushCompanionToDbAndReturnId(CompanionActionPtr action);

    std::shared_ptr<CompanionResult> addCompanionAndRetutnResult(int id, const std::string &name);
    ActionResultPtr addCompanionToNetworkManagerMap(int id, CompanionDataPtr data);
    void setCurrentAction(ActionPtr action);
    void checkAndResetCurrentAction(ActionPtr action);

    // message
    void sendUnsentMessages(CompanionPtr companion);

    // widgets
    void deleteWidgetGroupAndDeleteFromMapping(CompanionPtr companion);

    // files

    // network
    uint16_t getServerPortByCompanionId(int id);

    GraphicManagerPtr graphicManager_;
    DBOperator dbOperator_;
    MessageHandler messageHandler_;
    FileManagerPtr fileManager_;
    NetworkManagerPtr networkManager_;
    std::mutex messageStateToMessageMapMutex_;
    // bool userIsAuthenticated_;
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

ManagerPtr getManager();

#endif // MANAGER_HPP
