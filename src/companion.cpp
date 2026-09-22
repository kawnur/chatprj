#include "companion.hpp"

#include "action.hpp"
#include "chat_client.hpp"
#include "chat_server.hpp"
#include "data.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"

SocketInfo::SocketInfo(const SocketInfo &object)
{
    ipAddress_ = object.ipAddress_;
    serverPort_ = object.serverPort_;
    clientPort_ = object.clientPort_;
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

Companion::Companion(int id, const std::string &name)
    : mutex_(std::mutex()), id_(id), name_(name), socketInfo_(nullptr), client_(nullptr),
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

std::string Companion::getFileOperatorFilePathStringByNetworkId(const std::string &networkId)
{
    return fileOperatorStorage_->getOperator(networkId)->getPath().string();
}

bool Companion::removeOperatorFromStorage(const std::string &key)
{
    return fileOperatorStorage_->removeOperator(key);
}

MessageMappingIterator Companion::getMessageMappingIteratorByMessage(
    std::shared_ptr<Message> message)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&](const auto &iter)
    {
        return iter.second->getMessage() == message;
    };

    return std::ranges::find_if(messageMapping_, lambda);
}

std::shared_ptr<MessageWidget> Companion::getMappedMessageWidgetByMessage(
    std::shared_ptr<Message> message)
{
    auto result = getMessageMappingIteratorByMessage(message);

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
        std::lock_guard<std::mutex> lockObject(mutex_);

    auto lambda = [&](const auto &iter) { return iter.second->getWidget().get() == widget; };
    auto result = std::ranges::find_if(messageMapping_, lambda);

    return (result == messageMapping_.end()) ? nullptr : result->second->getMessage();
}

std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessageWidget(
    bool lock, std::shared_ptr<MessageWidget> widget)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    auto lambda = [&](const auto &iter)
    {
        return iter.second->getWidget() == widget;
    };

    auto result = std::ranges::find_if(messageMapping_, lambda);

    return (result == messageMapping_.end()) ? nullptr : result->second->getMessage()->state();
}

std::shared_ptr<MessageInfo> Companion::getMessageInfoByMessageId(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&](const auto &iter)
    {
        return iter.second->getMessage()->getId() == messageId;
    };

    auto result = std::ranges::find_if(messageMapping_, lambda);

    return (result == messageMapping_.end()) ? nullptr : result->second;
}

std::shared_ptr<MessageInfo> Companion::getMessageInfoByNetworkId(const std::string &networkId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [&](const auto &iter)
    {
        return iter.second->getMessage()->getNetworkId() == networkId;
    };

    auto result = std::ranges::find_if(messageMapping_, lambda);

    return (result == messageMapping_.end()) ? nullptr : result->second;
}

std::shared_ptr<Message> Companion::getEarliestMessage() const
{
    auto lambda = [&](const auto &iterator1, const auto &iterator2)
    {
        return iterator1.second->getMessage()->getId() < iterator2.second->getMessage()->getId();
    };

    auto min = std::ranges::min_element(messageMapping_, lambda);

    return min->second->getMessage();
}

std::shared_ptr<Message> Companion::createMessage(
    // MessageType type, /*uint32_t messageId, uint8_t authorId, const std::string &messageTime,*/
    // std::shared_ptr<MessageMetaData> meta, const std::string &messageText, bool isAntecedent,
    // bool isSent, bool isReceived, std::string networkId)
    std::shared_ptr<MessageMetaData> meta, std::shared_ptr<MessageData> data,
    std::shared_ptr<MessageState> state)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (meta->networkId_.size() == 0)
        meta->networkId_ = generateNetworkId(false);

    // auto messageState = std::make_shared<MessageState>(
    //     companionId, isAntecedent, isSent, isReceived, networkId);

    auto message = std::make_shared<Message>(meta, data, state);
    auto info = std::make_shared<MessageInfo>(message, nullptr);
    auto result = messageMapping_.emplace(meta->messageId_, info);

    // return result;
    return (result.second) ? message : nullptr;
}

std::shared_ptr<MessageInfo> Companion::createMessageAndAddToMapping(
    std::shared_ptr<DBReplyData> messagesData, std::size_t index)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto id = id_;

    auto meta = std::make_shared<MessageMetaData>(
        MessageType::TEXT, NetworkMessageType::UNKNOWN,
        std::stoi(messagesData->getValue(index, "id")), id, name_,
        std::stoi(messagesData->getValue(index, "author_id")), "",
        messagesData->getValue(index, "timestamp_tz"), "", generateNetworkId(false));

    auto data = std::make_shared<MessageData>(messagesData->getValue(index, "message"), "");

    auto state = std::make_shared<MessageState>(
        false,
        getBoolFromDBValue(messagesData->getValue(index, "is_sent")),
        getBoolFromDBValue(messagesData->getValue(index, "is_received")));

    auto message = std::make_shared<Message>(meta, data, state);
    auto info = std::make_shared<MessageInfo>(message, nullptr);
    auto result = messageMapping_.emplace(std::make_pair(message->getId(), info));

    if (!result.second)
        logArgsError("message info map emplacing error");

    return (result.second) ? info : nullptr;
}

void Companion::setSocketInfo(std::shared_ptr<SocketInfo> socketInfo)
{
    socketInfo_ = socketInfo;
}

bool Companion::setFileOperatorFilePath(
    const std::string &networkId, const std::filesystem::path &path)
{
    return fileOperatorStorage_->getOperator(networkId)->setPath(path);
}

void Companion::setMappedMessageWidget(
    std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto id = message->getId();
    auto result = messageMapping_.find(id);

    if (result == messageMapping_.end())
        logTemplateError("message with id {} was not found in messageMapping_", id);
    else
        result->second->setWidget(widget);
}

bool Companion::startServer()
{
    bool started = false;

    auto lambda = [&]()
    {
        server_ = std::make_shared<ChatServer>(shared_from_this(), socketInfo_->getServerPort());
        server_->run();
        started = true;
    };

    runAndLogException(lambda);

    return started;
}

bool Companion::createClient()
{
    bool created = false;

    auto lambda = [&]()
    {
        client_ = std::make_shared<ChatClient>(
            socketInfo_->getIpAddress(), socketInfo_->getClientPort());

        created = true;
    };

    runAndLogException(lambda);

    return created;
}

bool Companion::connectClient()
{
    return client_->connect();
}

bool Companion::disconnectClient()
{
    return client_->disconnect();
}

bool Companion::sendMessage(
    // bool isAntecedent, NetworkMessageType type, std::string networkId,
    // std::shared_ptr<Message> message)
    std::shared_ptr<Message> message, std::shared_ptr<MessageMetaData> meta)
{
    if (meta->networkMessageType_ == NetworkMessageType::UNKNOWN || !client_)
        return false;

    if (meta->networkMessageType_ == NetworkMessageType::NO_ACTION)
        return true;

    // check client
    bool isConnected = client_->isConnected();

    if (!isConnected)
        return false;

    // build json
    auto json = buildMessageJSONString(shared_from_this(), message, meta);

    // send json over network
    auto result = client_->send(json);

    if (!result)
        logArgsError("client message sending error");

    return result;
}

bool Companion::sendChatHistory(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys) const
{
    if (!client_)
        return false;

    bool isConnected = client_->isConnected();

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

    auto meta = std::make_shared<MessageMetaData>();
    meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;

    bool result = sendMessage(message, meta);

    return result;
}

bool Companion::sendFileBlock(const std::string &networkId, const std::string &data)
{
    bool isConnected = client_->isConnected();

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
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = messageMapping_.find(messageId);

    return (result == messageMapping_.end()) ? nullptr : result->second->getMessage();
}

void Companion::addMessageWidgetsToChatHistory()
{
    auto widgetGroup = getManager()->getMappedWidgetGroupByCompanion(shared_from_this());

    std::lock_guard<std::mutex> lock(mutex_);

    for (auto &iterator : messageMapping_)
        widgetGroup->addMessageWidgetToCentralPanelChatHistory(iterator.second->getMessage());
}

void Companion::clearMessageMapping()
{
    std::lock_guard<std::mutex> lock(mutex_);

    messageMapping_.clear();
}

void Companion::addReceiverOperator(
    std::shared_ptr<MessageMetaData> meta, const std::filesystem::path &path)
{
    fileOperatorStorage_->addReceiverOperator(meta, path);
}

std::string Companion::generateNetworkId(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    std::string networkId { "" };

    if (networkId.size() == 0) {  // TODO ???
        networkId = getRandomString(5);

        auto lambda = [&]()
        {
            auto lambdaInternal = [&](const auto &iter)
            {
                return iter.second->getMessage()->getNetworkId() == networkId;
            };

            auto iterator = std::ranges::find_if(messageMapping_, lambdaInternal);

            return !(iterator == messageMapping_.end());
        };

        // loop while generated key is not unique
        while (lambda())
            networkId = getRandomString(5);
    }

    return networkId;
}

bool validateCompanionData(
    std::vector<std::string> &errors, std::shared_ptr<CompanionAction> action)
{
    bool nameValidationResult = validateCompanionName(errors, action->getName());
    bool ipAddressValidationResult = validateIpAddress(errors, action->getIpAddress());
    bool portValidationResult = validatePort(errors, action->getClientPort());

    bool result = nameValidationResult && ipAddressValidationResult && portValidationResult;

    logArgs("validateCompanionData result:", result);

    return result;
}

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    auto companionName = companion->getName();
    auto companionId = message->getCompanionId();
    auto authorId = message->getAuthorId();
    auto time = message->getTime();
    auto text = message->getText();

    std::string color, sender, receiver;

    if (companionId == authorId) {
        color = receivedMessageColor;
        sender = companionName;
        receiver = "Me";
    }
    else {
        color = sentMessageColor;
        sender = "Me";
        receiver = companionName;
    }

    auto header = getStringByFormat(
        "<font color=\"{0}\"><b><br><i>From {1} to {2} at {3}:</i></b></font>",
        color, sender, receiver, time);

    std::string body = getFormattedMessageBodyString(color, text);

    std::pair<std::string, std::string> data (header, body);

    return data;
}

std::string buildMessageJSONString(
    // bool isAntecedent, NetworkMessageType type, std::shared_ptr<Companion> companion,
    // const std::string &networkId, std::shared_ptr<Message> message)
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    std::shared_ptr<MessageMetaData> meta)
{
    using json = nlohmann::json;

    json jsonData;

    auto networkId = message->getNetworkId();
    auto type = meta->networkMessageType_;

    jsonData["type"] = type;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->getId();
    jsonData["antecedent"] = message->isAntecedent();

    switch (type) {
    case NetworkMessageType::TEXT: {
        jsonData["time"] = message->getTime();
        jsonData["text"] = message->getText();
    }

    break;

    case NetworkMessageType::FILE_PROPOSAL: {
        jsonData["time"] = message->getTime();
        jsonData["text"] = message->getText();
        jsonData["hashMD5"] =
            companion->getFileOperatorStorage()->getOperator(networkId)->getMD5Hash();
    }

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        jsonData["received"] = 1;

        break;

    default:
        break;
    }

    return jsonData.dump();
}

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::FILE_DATA;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->getId();
    jsonData["data"] = data;

    return jsonData.dump();
}
