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
#include "data_checker.hpp"
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

    MessagePtr pushMessageToDB(
        MessagePtr message, const std::string &companionName, const std::string &authorName);

    bool pushMessageHistoryToDb(CompanionPtr companion, const nlohmann::json &data);
    DBReplyDataPtr getEarlyMessagesDBData(int companionId, int messageId);

    // action
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

    ActionResultPtr createCompanion(CreateCompanionActionPtr action);
    ActionResultPtr updateCompanion(UpdateCompanionActionPtr action);
    ActionResultPtr deleteCompanion(DeleteCompanionActionPtr action);
    ActionResultPtr clearCompanionHistory(ClearHistoryCompanionActionPtr action);
    ActionResultPtr createUserPassword(CreatePasswordActionPtr action);
    ActionResultPtr authenticateUser(GetPasswordActionPtr action);

    // message
    void receiveMessage(int companionId, const std::string &json);
    void requestHistoryFromCompanion(CompanionPtr companion);
    void sendHistoryToCompanion(CompanionPtr companion);
    void sendTextMessage(CompanionPtr companion, ActionPtr action, const std::string &text);
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

    // void addMessageWidget(CompanionPtr companion, MessagePtr message);
    template<typename T>
    void addMessageWidget(CompanionPtr companion, MessagePtr message)
    {
        graphicManager_->addMessageWidget<T>(companion, message);
    }

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
    // void receiveFile(CompanionPtr companion, MessagePtr message);
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
    ActionResultPtr updateCompanionInDbAndReturnId(UpdateCompanionActionPtr action);
    ActionResultPtr deleteCompanionMessagesFromDbAndReturnId(uint32_t companionId);
    ActionResultPtr deleteCompanionAndSocketFromDbAndReturnId(DeleteCompanionActionPtr action);

    // action
    template<typename T, typename... Ts>
    void startAction(Ts &&...args)
    {
        auto action = std::make_shared<T>(shared_from_this(), args...);
        setCurrentAction(std::dynamic_pointer_cast<Action>(action));  // TODO check cast
        action->setDialog(action);
        action->preAct();
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

    ActionResultPtr pushPasswordToDbAndReturnId(PasswordActionPtr action);
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
    DataChecker dataChecker_;
    MessageHandler messageHandler_;
    FileManagerPtr fileManager_;
    NetworkManagerPtr networkManager_;
    std::mutex messageStateToMessageMapMutex_;
    // bool userIsAuthenticated_;
    ActionPtr currentAction_;
};

ManagerPtr getManager();

#endif // MANAGER_HPP
