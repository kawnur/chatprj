#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>

#include "constants.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "widgets_message.hpp"

class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionData;
class DBReplyData;
class FileOperatorStorage;
class FileMessageWidget;
class Message;
class MessageInfo;
class MessageState;
class WidgetGroup;

template<typename... Ts> void logArgsInfoWithTemplate(
    const std::format_string<Ts...>&, Ts&&...);

template<typename... Ts> void logArgsErrorWithTemplate(
    const std::format_string<Ts...>&, Ts&&...);

class SocketInfo {
public:
    SocketInfo() {}
    SocketInfo(std::string&, uint16_t&, uint16_t&);  // TODO remove
    SocketInfo(std::string&&, uint16_t&&, uint16_t&&);  // TODO remove

    template<typename T, typename U, typename P> SocketInfo
        (T&& ipAddress, U&& serverPort, P&& clientPort) :
        ipAddress_(std::forward<T>(ipAddress)),
        serverPort_(std::forward<U>(serverPort)),
        clientPort_(std::forward<P>(clientPort)) {}

    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {}
    ~SocketInfo() = default;

    std::string getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    void updateData(std::shared_ptr<CompanionData>);

private:
    std::string ipAddress_;
    uint16_t serverPort_;  // port number to open server at
    uint16_t clientPort_;  // port number to connect with client to
};

class Companion {
public:
    Companion(int, const std::string&);
    ~Companion();

    friend bool operator<(const Companion& object1, const Companion& object2) {
        return object1.getId() < object2.getId();
    }

    int getId() const;
    std::string getName() const;
    std::shared_ptr<SocketInfo> getSocketInfoPtr() const;
    std::string getSocketIpAddress() const;
    uint16_t getSocketServerPort() const;
    uint16_t getSocketClientPort() const;
    std::shared_ptr<FileOperatorStorage> getFileOperatorStoragePtr() const;
    std::string getFileOperatorFilePathStringByNetworkId(const std::string&);
    bool removeOperatorFromStorage(const std::string&);

    template<typename T>
    std::shared_ptr<T> getFileOperatorPtrByNetworkId(const std::string& networkId) {
        return dynamic_cast<std::shared_ptr<T>>(this->fileOperatorStoragePtr_->getOperator(networkId));
    }

    template<typename T>
    void removeFileOperator(const std::string& networkId) {
        auto operatorPtr =
            this->getFileOperatorPtrByNetworkId<T>(networkId);

        if(operatorPtr) {
            if(!this->removeOperatorFromStorage(networkId)) {
                logArgsInfoWithTemplate(
                    "remove file operator error for networkId {}", networkId);
            }

            if(operatorPtr) {
                delete operatorPtr;
            }

            logArgsInfoWithTemplate(
                "file operator for networkId {} deleted", networkId);
        }
        else {
            logArgsErrorWithTemplate(
                "file operator was not found for networkId {}", networkId);
        }
    }

    std::shared_ptr<MessageState> getMappedMessageStatePtrByMessagePtr(std::shared_ptr<Message>);
    std::shared_ptr<MessageWidget> getMappedMessageWidgetPtrByMessagePtr(std::shared_ptr<Message>);
    std::shared_ptr<Message> getMappedMessagePtrByMessageWidgetPtr(bool, std::shared_ptr<MessageWidget>);
    std::shared_ptr<MessageState> getMappedMessageStatePtrByMessageWidgetPtr(bool, std::shared_ptr<MessageWidget>);

    std::pair<const Message, MessageInfo>* getMessageMappingPairPtrByMessageId(uint32_t);

    std::pair<const Message, MessageInfo>* getMessageMappingPairPtrByNetworkId(
        const std::string&);

    std::shared_ptr<Message> getEarliestMessagePtr() const;

    std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
    createMessageAndAddToMapping(
        MessageType, uint32_t, uint8_t, const std::string&, const std::string&,
        bool, bool, bool, std::string);

    std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
    createMessageAndAddToMapping(
        std::shared_ptr<DBReplyData>&, std::size_t);

    void setSocketInfo(std::shared_ptr<SocketInfo>);
    bool setFileOperatorFilePath(const std::string&, const std::filesystem::path&);
    void setMappedMessageWidget(std::shared_ptr<Message>, std::shared_ptr<MessageWidget>);
    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();
    bool sendMessage(bool, NetworkMessageType, std::string, std::shared_ptr<Message>);
    bool sendChatHistory(std::shared_ptr<DBReplyData>&, std::vector<std::string>&) const;
    bool sendFileRequest(std::shared_ptr<FileMessageWidget>);
    bool sendFileBlock(const std::string&, const std::string&);
    void updateData(std::shared_ptr<CompanionData>);
    std::shared_ptr<Message> findMessage(uint32_t);
    void addMessageWidgetsToChatHistory();
    void clearMessageMapping();

private:
    std::mutex messagesMutex_;
    int id_;  // TODO change type
    std::string name_;
    std::shared_ptr<SocketInfo> socketInfoPtr_;
    std::shared_ptr<ChatClient> clientPtr_;
    std::shared_ptr<ChatServer> serverPtr_;
    std::shared_ptr<FileOperatorStorage> fileOperatorStoragePtr_;
    std::map<Message, MessageInfo> messageMapping_;

    std::string generateNewNetworkId(bool);
};

#endif // COMPANION_HPP
