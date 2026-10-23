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

int getDataFromDBResult(bool log, DBReplyDataPtr data, PGresultPtr result, int maxTuples);

// class Manager : public QObject // TODO do we need inheritance?
class Manager : public QObject, public std::enable_shared_from_this<Manager>
{
public:
    Manager(GraphicManagerPtr graphicManager);
    ~Manager();

    template<typename... Ts>
    DBReplyDataPtr getDBData(DBRequestType type, Ts &&...args)
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
    bool pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data);
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
    void initFileSend(CompanionPtr companion);
    void initFileReception(const std::string &networkId, CompanionPtr companion);
    ActionResultPtr createCompanion(CompanionActionPtr action);
    ActionResultPtr updateCompanion(CompanionActionPtr action);
    // void updateSocketInfo(int id, CompanionDataPtr data);
    ActionResultPtr deleteCompanion(CompanionActionPtr action);
    void clearChatHistory(CompanionPtr companion);
    ActionResultPtr clearCompanionHistory(CompanionActionPtr action);
    ActionResultPtr pushPasswordToDbAndReturnId(PasswordActionPtr action);
    ActionResultPtr createUserPassword(PasswordActionPtr action);
    ActionResultPtr authenticateUser(PasswordActionPtr action);
    void startUserAuthentication();
    void requestHistoryFromCompanion(CompanionPtr companion);
    void sendChatHistoryToCompanion(CompanionPtr companion);
    // bool isInitialised();
    std::filesystem::path getLastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);
    void endAction(ActionPtr action);

    void sendMessage(
        MessageType type, CompanionPtr companion, ActionPtr action, const std::string &text);

    bool sendMessage(CompanionPtr companion, MessagePtr message);
    bool sendFileRequestMessage(CompanionPtr companion, const std::string &networkId);

    bool sendFileDataTransmissionResultMessage(
        CompanionPtr companion, NetworkMessageType type, const std::string &networkId);

    MessagePtr pushMessageToDB(MessagePtr message);
    void markMessageAsSent(CompanionPtr companion, MessagePtr message);
    void markMessageAsReceived(MessageInfoPtr info);
    void addEarlyMessages(CompanionPtr companion);
    void sendUnsentMessages(CompanionPtr companion);

    bool sendFileBlock(
        CompanionPtr companion, const std::string &networkId, const std::string &data);

    CompanionDataPtr getCompanionData(CompanionPtr companion);
    bool connectClient(CompanionPtr companion);
    bool disconnectClient(CompanionPtr companion);
    void addSenderOperator(const std::string &networkId, ActionPtr action);
    void addReceiverOperator(MessagePtr message);
    void removeSenderOperator(const std::string &networkId);
    std::string getFilePathString(const std::string &networkId);
    bool setFileOperatorFilePath(const std::string &networkId, const std::filesystem::path &path);
    std::string getFileOperatorMD5Hash(const std::string &networkId);

    bool setMappedMessageWidget(
        CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget);

    void fillCompanionMessageMapping(CompanionPtr companion, bool containersNotEmpty);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    void sortHistory(CompanionPtr companion);
    void disableBlur();
    bool clientAction(CompanionPtr companion, bool isConnected);
    void sendFile(CompanionPtr companion, MessagePtr message);
    void receiveFileData(CompanionPtr companion, MessagePtr message);
    void receiveFile(CompanionPtr companion, MessagePtr message);
    std::optional<bool> stopFileDataReceiving(MessagePtr message);
    void stopFileDataSending(MessagePtr message);
    bool sendJsonByNetwork(CompanionPtr companion, const std::string &json);

private:
    ActionResultPtr getActionResultByKeyDBData(
        DBReplyDataPtr data, const std::string &key, bool allowEmptyResult = false);

    ActionResultPtr pushCompanionToDbAndReturnId(CompanionActionPtr action);
    uint16_t getServerPortByCompanionId(int id);
    ActionResultPtr pushSocketToDb(CompanionActionPtr action, int serverPort);
    ActionResultPtr updateCompanionInDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionMessagesFromDbAndReturnId(CompanionActionPtr action);
    ActionResultPtr deleteCompanionAndSocketFromDbAndReturnId(CompanionActionPtr action);
    bool buildCompanions();
    CompanionPtr addCompanion(int id, const std::string &name);
    CompanionPtr addCompanion(int id, const std::string &name, std::string &entry);
    std::shared_ptr<CompanionResult> addCompanionAndRetutnResult(int id, const std::string &name);
    ActionResultPtr addCompanionToNetworkManagerMap(int id, CompanionDataPtr data);
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

    GraphicManagerPtr graphicManager_;
    DBOperator dbOperator_;
    MessageHandler messageHandler_;
    FileManager fileManager_;
    NetworkManagerPtr networkManager_;
    std::mutex messageStateToMessageMapMutex_;
    bool userIsAuthenticated_;
    CompanionPtr selectedCompanion_;
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
