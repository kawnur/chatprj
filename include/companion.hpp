#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <utility>

#include "constants.hpp"
#include "file_info.hpp"

class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionData;
class DBReplyData;
class FileOperatorStorage;
class FileMessageWidget;
class Message;
class MessageInfo;
class MessageMetaData;
class MessageState;
class MessageWidget;
class WidgetGroup;

template<typename T, typename... Ts>
void logTemplateInfo(T &&templateString, Ts &&...args);

template<typename T, typename... Ts>
void logTemplateError(T &&templateString, Ts &&...args);

using MessageMapping = std::map<std::shared_ptr<Message>, std::shared_ptr<MessageInfo>>;
using MessageMappingIterator = MessageMapping::iterator;
using MessageMappingPair = std::pair<std::shared_ptr<Message>, std::shared_ptr<MessageInfo>>;

class SocketInfo
{
public:
    SocketInfo() = default;

    template<typename T, typename U, typename P>
    SocketInfo(T &&ipAddress, U &&serverPort, P &&clientPort)
        : ipAddress_(std::forward<T>(ipAddress)), serverPort_(std::forward<U>(serverPort)),
        clientPort_(std::forward<P>(clientPort)) {}

    SocketInfo(const SocketInfo &object);
    SocketInfo(SocketInfo &&object) = default;
    ~SocketInfo() = default;

    std::string getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    void updateData(std::shared_ptr<CompanionData> data);

private:
    std::string ipAddress_;
    uint16_t serverPort_;  // port number to open server at
    uint16_t clientPort_;  // port number to connect with client to
};

class Companion : public std::enable_shared_from_this<Companion>
{
public:
    Companion(int id, const std::string &name);
    ~Companion() = default;

    friend bool operator<(const Companion &object1, const Companion &object2)
    {
        return object1.getId() < object2.getId();
    }

    int getId() const;
    std::string getName() const;
    std::shared_ptr<SocketInfo> getSocketInfo() const;
    std::string getSocketIpAddress() const;
    uint16_t getSocketServerPort() const;
    uint16_t getSocketClientPort() const;
    std::shared_ptr<FileOperatorStorage> getFileOperatorStorage() const;
    std::string getFileOperatorFilePathStringByNetworkId(const std::string &networkId);
    bool removeOperatorFromStorage(const std::string &key);

    template<typename T>
    std::shared_ptr<T> getFileOperatorByNetworkId(const std::string &networkId)
    {
        return dynamic_pointer_cast<T>(fileOperatorStorage_->getOperator(networkId));
    }

    template<typename T>
    void removeFileOperator(const std::string &networkId)
    {
        auto fileOperator = getFileOperatorByNetworkId<T>(networkId);

        if (fileOperator) {
            logTemplateError("file operator was not found for networkId {}", networkId);

            return;
        }

        if (!removeOperatorFromStorage(networkId))
            logTemplateInfo("remove file operator error for networkId {}", networkId);

        // if (fileOperator)
        //     delete fileOperator;  // TODO remove

        // logTemplateInfo("file operator for networkId {} deleted", networkId);
    }

    MessageMappingIterator getMessageMappingIteratorByMessage(std::shared_ptr<Message> message);
    std::shared_ptr<MessageState> getMappedMessageStateByMessage(std::shared_ptr<Message> message);
    std::shared_ptr<MessageWidget> getMappedMessageWidgetByMessage(std::shared_ptr<Message> message);

    std::shared_ptr<Message> getMappedMessageByMessageWidget(
        bool lock, std::shared_ptr<MessageWidget> widget);

    std::shared_ptr<Message> getMappedMessageByMessageWidget(bool lock, MessageWidget *widget);

    std::shared_ptr<MessageState> getMappedMessageStateByMessageWidget(
        bool lock, std::shared_ptr<MessageWidget> widget);

    MessageMappingPair getMessageMappingPairByMessageId(uint32_t messageId);
    MessageMappingPair getMessageMappingPairByNetworkId(const std::string &networkId);
    std::shared_ptr<Message> getEarliestMessage() const;

    std::pair<MessageMappingIterator, bool> createMessageAndAddToMapping(
        MessageType type, /*uint32_t messageId, uint8_t authorId, const std::string &messageTime,*/
        const MessageMetaData &meta, const std::string &messageText, bool isAntecedent, bool isSent, bool isReceived,
        std::string networkId);

    std::pair<MessageMappingIterator, bool> createMessageAndAddToMapping(
        std::shared_ptr<DBReplyData> messagesData, std::size_t index);

    void setSocketInfo(std::shared_ptr<SocketInfo> socketInfo);
    bool setFileOperatorFilePath(const std::string &networkId, const std::filesystem::path &path);

    void setMappedMessageWidget(
        std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget);

    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();

    bool sendMessage(
        bool isAntecedent, NetworkMessageType type, std::string networkId,
        std::shared_ptr<Message> message);

    bool sendChatHistory(std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys) const;
    bool sendFileRequest(std::shared_ptr<FileMessageWidget> widget);
    bool sendFileBlock(const std::string &networkId, const std::string &data);
    void updateData(std::shared_ptr<CompanionData> data);
    std::shared_ptr<Message> findMessage(uint32_t messageId);
    void addMessageWidgetsToChatHistory();
    void clearMessageMapping();

private:
    std::string generateNewNetworkId(bool);

    std::mutex mutex_;
    int id_;  // TODO change type
    std::string name_;
    std::shared_ptr<SocketInfo> socketInfo_;
    std::shared_ptr<ChatClient> client_;
    std::shared_ptr<ChatServer> server_;
    std::shared_ptr<FileOperatorStorage> fileOperatorStorage_;
    MessageMapping messageMapping_;
};

#endif // COMPANION_HPP
