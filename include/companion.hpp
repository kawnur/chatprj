#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>

#include "constants.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "message.hpp"

class CentralPanelWidget;
class ChatClient;
class ChatServer;
class CompanionData;
class DBReplyData;
class Message;
class MessageInfo;
class WidgetGroup;

class SocketInfo
{
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

    void updateData(const CompanionData*);

private:
    std::string ipAddress_;
    uint16_t serverPort_;  // port number to open server at
    uint16_t clientPort_;  // port number to connect with client to
};

class Companion
{
public:
    Companion(int, const std::string&);
    // Companion(int, std::string&&);
    ~Companion();

    int getId() const;
    std::string getName() const;
    SocketInfo* getSocketInfoPtr() const;
    std::string getSocketIpAddress() const;
    uint16_t getSocketServerPort() const;
    uint16_t getSocketClientPort() const;
    // const std::vector<Message*>* getMessagePointersPtr() const;

    const MessageState* getMappedMessageStateByMessagePtr(const Message*);

    std::pair<const Message, MessageInfo>* getMessageMappingPairPtrByMessageMappingKey(
        const std::string&);

    std::pair<const Message, MessageInfo>* getMessageMappingPairPtrByMessageId(uint32_t);
    void createMessageAndAddToContainers(std::shared_ptr<DBReplyData>&, size_t);

    void setSocketInfo(SocketInfo*);

    bool startServer();
    bool createClient();
    bool connectClient();
    bool disconnectClient();
    // void clearMessages();
    // void addMessage(Message*);
    bool sendMessage(bool, NetworkMessageType, std::string, const Message*);
    bool sendChatHistory(std::shared_ptr<DBReplyData>&, std::vector<std::string>&) const;
    void updateData(const CompanionData*);
    Message* findMessage(uint32_t);
    void addMessageWidgetsToChatHistory(const WidgetGroup*, CentralPanelWidget*);

private:
    std::mutex messagesMutex_;
    int id_;  // TODO change type
    std::string name_;
    SocketInfo* socketInfoPtr_;
    ChatClient* clientPtr_;
    ChatServer* serverPtr_;
    // std::vector<Message*>* messagePointersPtr_;
    std::map<Message, MessageInfo> messageMapping_;
};

#endif // COMPANION_HPP
