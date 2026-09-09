#include "companion.hpp"

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "logging.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"

SocketInfo::SocketInfo(std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort)
    : ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

SocketInfo::SocketInfo(
    std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort)  // TODO ???
    : ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

SocketInfo::SocketInfo(const SocketInfo& si)
{
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;
}

std::string SocketInfo::getIpAddress() const
{
    return ipAddress_;
}

uint16_t SocketInfo::getServerPort() const
{
    return serverPort_;
}

uint16_t SocketInfo::getClientPort() const
{
    return clientPort_;
}

void SocketInfo::updateData(std::shared_ptr<CompanionData> data)
{
    ipAddress_ = data->getIpAddress();
    clientPort_ = std::stoi(data->getClientPort());
}

Companion::Companion(int id, const std::string& name)
    : messagesMutex_(std::mutex()), id_(id), name_(name), socketInfo_(nullptr), client_(nullptr),
    server_(nullptr), messageMapping_(), fileOperatorStorage_(new FileOperatorStorage) {}

int Companion::getId() const
{
    return id_;
}

std::string Companion::getName() const
{
    return name_;
}

std::shared_ptr<SocketInfo> Companion::getSocketInfo() const
{
    return socketInfo_;
}

std::string Companion::getSocketIpAddress() const
{
    return socketInfo_->getIpAddress();
}

uint16_t Companion::getSocketServerPort() const
{
    return socketInfo_->getServerPort();
}

uint16_t Companion::getSocketClientPort() const
{
    return socketInfo_->getClientPort();
}

std::shared_ptr<FileOperatorStorage> Companion::getFileOperatorStorage() const
{
    return fileOperatorStorage_;
}

std::string Companion::getFileOperatorFilePathStringByNetworkId(const std::string& networkId)
{
    return fileOperatorStorage_->getOperator(networkId)->getFilePath().string();
}

bool Companion::removeOperatorFromStorage(const std::string& key)
{
    return fileOperatorStorage_->removeOperator(key);
}

std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessage(
    std::shared_ptr<Message> message)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iter){
            return iter.first == message;
        });

    return (result == messageMapping_.end()) ? nullptr : result->second->getState();
}

std::shared_ptr<MessageWidget> Companion::getMappedMessageWidgetByMessage(
    std::shared_ptr<Message> message)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iter){
            return iter.first == message;
        });

    return (result == messageMapping_.end()) ? nullptr : result->second->getWidget();
}

std::shared_ptr<Message> Companion::getMappedMessageByMessageWidget(
    bool lock, std::shared_ptr<MessageWidget> widget)
{
    return getMappedMessageByMessageWidget(lock, widget.get());
}

std::shared_ptr<Message> Companion::getMappedMessageByMessageWidget(
    bool lock, MessageWidget *widget)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(messagesMutex_);

    // TODO switch to map find method

    auto lambda = [&](auto& iter) { return iter.second->getWidget().get() == widget; };
    auto result = std::find_if (messageMapping_.begin(), messageMapping_.end(), lambda);

    return (result == messageMapping_.end()) ? nullptr : result->first;
}

std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessageWidget(
    bool lock, std::shared_ptr<MessageWidget> widget)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iter){
            return iter.second->getWidget() == widget;
        });

    return (result == messageMapping_.end()) ? nullptr : result->second->getState();
}

MessageMappingPair Companion::getMessageMappingPairByMessageId(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iter){
            return iter.first->getId() == messageId;
        });

    // return (result == messageMapping_.end())
    //            ? MessageMappingPair(nullptr, nullptr)
    //            : std::make_pair<std::shared_ptr<Message>, std::shared_ptr<MessageInfo>>(result->first, result->second);

    if (result == messageMapping_.end())
        return MessageMappingPair(nullptr, nullptr);

    auto pair = MessageMappingPair(result->first, result->second);

    return pair;
}

MessageMappingPair Companion::getMessageMappingPairByNetworkId(const std::string& networkId)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iter){
            return iter.second->getState()->getNetworkId() == networkId;
        });

    if (result == messageMapping_.end())
        return MessageMappingPair(nullptr, nullptr);

    auto pair = MessageMappingPair(result->first, result->second);

    return pair;
}

std::shared_ptr<Message> Companion::getEarliestMessage() const
{
    auto minPair = std::min_element(
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto& iterator1, auto& iterator2){
            return iterator1.first->getId() < iterator2.first->getId();
        });

    return minPair->first;
}

std::pair<MessageMappingIterator, bool> Companion::createMessageAndAddToMapping(
    MessageType type, uint32_t messageId, uint8_t authorId, const std::string& messageTime,
    const std::string& messageText, bool isAntecedent, bool isSent, bool isReceived,
    std::string networkId)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    if (networkId.size() == 0)
        networkId = generateNewNetworkId(false);

    auto companionId = getId();

    auto messageState = std::make_shared<MessageState>(
        companionId, isAntecedent, isSent, isReceived, networkId);

    auto message = std::make_shared<Message>(
        type, messageId, companionId, authorId, messageTime, messageText);

    auto messageInfo = std::make_shared<MessageInfo>(messageState, nullptr);

    auto result = messageMapping_.emplace(std::make_pair(message, messageInfo));

    return result;
}

std::pair<MessageMappingIterator, bool> Companion::createMessageAndAddToMapping(
    std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    auto id = getId();

    auto messageState = std::make_shared<MessageState>(
        id, false,
        getBoolFromDBValue(messagesData->getValue(index, "is_sent")),
        getBoolFromDBValue(messagesData->getValue(index, "is_received")),
        generateNewNetworkId(false));

    auto message = std::make_shared<Message>(
        MessageType::TEXT,
        std::stoi(messagesData->getValue(index, "id")),
        id,
        std::stoi(messagesData->getValue(index, "author_id")),
        messagesData->getValue(index, "timestamp_tz"),
        messagesData->getValue(index, "message")
    );

    auto messageInfo = std::make_shared<MessageInfo>(messageState, nullptr);

    auto result = messageMapping_.emplace(std::make_pair(message, messageInfo));

    return result;
}

void Companion::setSocketInfo(std::shared_ptr<SocketInfo> socketInfo)
{
    socketInfo_ = socketInfo;
}

bool Companion::setFileOperatorFilePath(
    const std::string& networkId, const std::filesystem::path& path)
{
    return fileOperatorStorage_->getOperator(networkId)->setFilePath(path);
}

void Companion::setMappedMessageWidget(
    std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    auto result = messageMapping_.find(message);

    if (result == messageMapping_.end())
        logTemplateError("message with id {} was not found in messageMapping_", message->getId());
    else
        result->second->setWidget(widget);
}

bool Companion::startServer()
{
    bool started = false;

    auto startLambda = [this](bool &value)
    {
        server_ = std::make_shared<ChatServer>(shared_from_this(), socketInfo_->getServerPort());
        server_->run();
        value = true;
    };

    runAndLogException(startLambda, started);

    return started;
}

bool Companion::createClient()
{
    bool created = false;

    auto createLambda = [this](bool &value)
    {
        client_ = std::make_shared<ChatClient>(
            socketInfo_->getIpAddress(),
            socketInfo_->getClientPort());

        value = true;
    };

    runAndLogException(createLambda, created);

    return created;
}

bool Companion::connectClient() {
    return client_->connect();
}

bool Companion::disconnectClient()
{
    return client_->disconnect();
}

bool Companion::sendMessage(
    bool isAntecedent, NetworkMessageType type, std::string networkId,
    std::shared_ptr<Message> message)
{
    if (type == NetworkMessageType::NO_ACTION)
        return true;

    if (!client_)
        return false;

    bool isConnected = client_->getIsConnected();

    if (!isConnected)
        return false;

    // build json
    auto jsonData =
        buildMessageJSONString(isAntecedent, type, shared_from_this(), networkId, message);

    // send json over network
    auto result = client_->send(jsonData);

    if (!result)
        logArgsError("client message sending error");

    return result;
}

bool Companion::sendChatHistory(
    std::shared_ptr<DBReplyData> data, std::vector<std::string>& keys) const
{
    if (!client_)
        return false;

    bool isConnected = client_->getIsConnected();

    if (isConnected)
        return false;

    // build json
    std::string jsonData = buildChatHistoryJSONString(data, keys);

    // send json over network
    auto result = client_->send(jsonData);

    if (!result)
        logArgsError("client chat history sending error");

    return result;
}

bool Companion::sendFileRequest(std::shared_ptr<FileMessageWidget> widget)
{
    auto message = getMappedMessageByMessageWidget(true, widget);

    if (!message)
        return false;

    std::shared_ptr<MessageState> state = nullptr;

    {
        std::lock_guard<std::mutex> lock(messagesMutex_);

        try {
            state = messageMapping_.at(message)->getState();
        }
        catch(std::out_of_range) {}
    }

    if (!state)
        return false;

    bool result = sendMessage(
        false, NetworkMessageType::FILE_REQUEST, state->getNetworkId(), message);

    return result;
}

bool Companion::sendFileBlock(const std::string& networkId, const std::string& data)
{
    bool isConnected = client_->getIsConnected();

    if (!isConnected)
        return false;

    // build json
    auto jsonData = buildFileBlockJSONString(shared_from_this(), networkId, data);

    // send json over network
    bool result = client_->send(jsonData);

    if (!result)
        logArgsError("client message sending error");

    return result;
}

void Companion::updateData(std::shared_ptr<CompanionData> data)
{
    name_ = data->getName();
    socketInfo_->updateData(data);
}

std::shared_ptr<Message> Companion::findMessage(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    auto result = std::find_if (
        messageMapping_.begin(),
        messageMapping_.end(),
        [&](auto iter){
            return iter.first->getId() == messageId;
        });

    return (result == messageMapping_.end()) ? nullptr : result->first;
}

void Companion::addMessageWidgetsToChatHistory()
{
    auto widgetGroup = getManager()->getMappedWidgetGroupByCompanion(shared_from_this());

    std::lock_guard<std::mutex> lock(messagesMutex_);

    for (auto& iterator : messageMapping_) {
        widgetGroup->addMessageWidgetToCentralPanelChatHistory(
            iterator.first, iterator.second->getState());
    }
}

void Companion::clearMessageMapping()
{
    std::lock_guard<std::mutex> lock(messagesMutex_);

    messageMapping_.clear();
}

std::string Companion::generateNewNetworkId(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(messagesMutex_);

    std::string networkId { "" };

    if (networkId.size() == 0) {  // TODO ???
        networkId = getRandomString(5);

        auto lambda = [&](){
            auto iterator = std::find_if (
                messageMapping_.begin(),
                messageMapping_.end(),
                [&](auto iter){
                    return iter.second->getState()->getNetworkId() == networkId;
                });

            return !(iterator == messageMapping_.end());
        };

        // loop while generated key is not unique
        while (lambda())
            networkId = getRandomString(5);
    }

    return networkId;
}
