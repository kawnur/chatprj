#include "companion.hpp"

SocketInfo::SocketInfo(
    std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

SocketInfo::SocketInfo(
    std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :  // TODO ???
    ipAddress_(ipAddress),
    serverPort_(serverPort),
    clientPort_(clientPort) {}

SocketInfo::SocketInfo(const SocketInfo& si) {
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;
}

std::string SocketInfo::getIpAddress() const {
    return this->ipAddress_;
}

uint16_t SocketInfo::getServerPort() const {
    return this->serverPort_;
}

uint16_t SocketInfo::getClientPort() const {
    return this->clientPort_;
}

void SocketInfo::updateData(std::shared_ptr<CompanionData> data) {
    this->ipAddress_ = data->getIpAddress();
    this->clientPort_ = std::stoi(data->getClientPort());
}

Companion::Companion(int id, const std::string& name) :
    messagesMutex_(std::mutex()), id_(id), name_(name), socketInfo_(nullptr),
    client_(nullptr), server_(nullptr), messageMapping_(),
    fileOperatorStorage_(new FileOperatorStorage) {}

int Companion::getId() const {
    return this->id_;
}

std::string Companion::getName() const {
    return this->name_;
}

std::shared_ptr<SocketInfo> Companion::getSocketInfo() const {
    return this->socketInfo_;
}

std::string Companion::getSocketIpAddress() const {
    return this->socketInfo_->getIpAddress();
}

uint16_t Companion::getSocketServerPort() const {
    return this->socketInfo_->getServerPort();
}

uint16_t Companion::getSocketClientPort() const {
    return this->socketInfo_->getClientPort();
}

std::shared_ptr<FileOperatorStorage> Companion::getFileOperatorStorage() const {
    return this->fileOperatorStorage_;
}

std::string Companion::getFileOperatorFilePathStringByNetworkId(
    const std::string& networkId) {
    return this->fileOperatorStorage_->
        getOperator(networkId)->getFilePath().string();
}

bool Companion::removeOperatorFromStorage(const std::string& key) {
    return this->fileOperatorStorage_->removeOperator(key);
}

std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessage(
    std::shared_ptr<Message> message) {
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter){
            return iter.first == message;
        });

    return (result == this->messageMapping_.end()) ? nullptr : result->second->getState();
}

std::shared_ptr<MessageWidget> Companion::getMappedMessageWidgetByMessage(std::shared_ptr<Message> message) {
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter){
            return iter.first == message;
        });

    return (result == this->messageMapping_.end()) ? nullptr : result->second->getWidget();
}

std::shared_ptr<Message> Companion::getMappedMessageByMessageWidget(
    bool lock, std::shared_ptr<MessageWidget> widget)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    // TODO switch to map find method

    auto lambda = [&](auto& iter) { return iter.second->getWidget() == widget; };
    auto result = std::find_if(this->messageMapping_.begin(), this->messageMapping_.end(), lambda);

    return (result == this->messageMapping_.end()) ? nullptr : result->first;
}

std::shared_ptr<MessageState> Companion::getMappedMessageStateByMessageWidget(
    bool lock, std::shared_ptr<MessageWidget> widget) {
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter){
            return iter.second->getWidget() == widget;
        });

    return (result == this->messageMapping_.end()) ?
        nullptr : result->second->getState();
}

// std::pair<const Message, MessageInfo>* Companion::getMessageMappingPairByMessageId(
//     uint32_t messageId) {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);

//     auto result = std::find_if(
//         this->messageMapping_.begin(),
//         this->messageMapping_.end(),
//         [&](auto& iter){
//             return iter.first->getId() == messageId;
//         });

//     return (result == this->messageMapping_.end()) ? nullptr : *result;
// }

// std::pair<const Message, MessageInfo>* Companion::getMessageMappingPairByNetworkId(
//     const std::string& networkId) {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);

//     auto result = std::find_if(
//         this->messageMapping_.begin(),
//         this->messageMapping_.end(),
//         [&](auto& iter){
//             return iter.second->getState()->getNetworkId() == networkId;
//         });

//     return (result == this->messageMapping_.end()) ? nullptr : *result;
// }

std::shared_ptr<Message> Companion::getEarliestMessage() const {
    auto minPair = std::min_element(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iterator1, auto& iterator2){
            return iterator1.first->getId() < iterator2.first->getId();
        });

    return minPair->first;
}

// std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
// Companion::createMessageAndAddToMapping(
//     MessageType type, uint32_t messageId, uint8_t authorId, const std::string& messageTime,
//     const std::string& messageText, bool isAntecedent, bool isSent, bool isReceived,
//     std::string networkId)
// {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);

//     if(networkId.size() == 0)
//         networkId = this->generateNewNetworkId(false);

//     auto companionId = this->getId();

//     auto messageState = std::make_shared<MessageState>(
//         companionId, isAntecedent, isSent, isReceived, networkId);

//     auto message = std::make_shared<Message>(
//         type, messageId, companionId, authorId, messageTime, messageText);

//     auto messageInfo = std::make_shared<MessageInfo>(messageState, nullptr);

//     auto result = this->messageMapping_.emplace(std::make_pair(message, messageInfo));

//     return result;
// }

// std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
// Companion::createMessageAndAddToMapping(
//     std::shared_ptr<DBReplyData> messagesData, std::size_t index) {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);

//     auto id = this->getId();

//     auto messageState = std::make_shared<MessageState>(
//         id, false,
//         getBoolFromDBValue(messagesData->getValue(index, "is_sent")),
//         getBoolFromDBValue(messagesData->getValue(index, "is_received")),
//         this->generateNewNetworkId(false));

//     auto message = std::make_shared<Message>(
//         MessageType::TEXT,
//         std::atoi(messagesData->getValue(index, "id")),
//         id,
//         std::atoi(messagesData->getValue(index, "author_id")),
//         messagesData->getValue(index, "timestamp_tz"),
//         messagesData->getValue(index, "message")
//     );

//     auto messageInfo = std::make_shared<MessageInfo>(messageState, nullptr);

//     auto result = this->messageMapping_.emplace(std::make_pair(message, messageInfo));

//     return result;
// }

void Companion::setSocketInfo(std::shared_ptr<SocketInfo> socketInfo) {
    socketInfo_ = socketInfo;
}

bool Companion::setFileOperatorFilePath(
    const std::string& networkId, const std::filesystem::path& path) {
    return this->fileOperatorStorage_->
        getOperator(networkId)->setFilePath(path);
}

void Companion::setMappedMessageWidget(
    std::shared_ptr<Message> message, std::shared_ptr<MessageWidget> widget) {
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = this->messageMapping_.find(message);

    if(result == this->messageMapping_.end()) {
        logTemplateError(
            "message with id {} was not found in messageMapping_", message->getId());
    }
    else {
        result->second->setWidget(widget);
    }
}

bool Companion::startServer() {
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

bool Companion::createClient() {
    bool created = false;

    auto createLambda = [this](bool &value)
    {
        this->client_ = std::make_shared<ChatClient>(
            this->socketInfo_->getIpAddress(),
            this->socketInfo_->getClientPort());

        value = true;
    };

    runAndLogException(createLambda, created);

    return created;
}

bool Companion::connectClient() {
    return this->client_->connect();
}

bool Companion::disconnectClient() {
    return this->client_->disconnect();
}

bool Companion::sendMessage(
    bool isAntecedent, NetworkMessageType type, std::string networkId,
    std::shared_ptr<Message> message)
{
    if (type == NetworkMessageType::NO_ACTION)
        return true;

    if(!this->client_)
        return false;

    bool isConnected = this->client_->getIsConnected();

    if(!isConnected)
        return false;

    // build json
    auto jsonData =
        buildMessageJSONString(isAntecedent, type, shared_from_this(), networkId, message);

    // send json over network
    auto result = this->client_->send(jsonData);

    if(!result)
        logArgsError("client message sending error");

    return result;
}

bool Companion::sendChatHistory(
    std::shared_ptr<DBReplyData> data, std::vector<std::string>& keys) const
{
    if(!this->client_)
        return false;

    bool isConnected = this->client_->getIsConnected();

    if(isConnected)
        return false;

    // build json
    std::string jsonData = buildChatHistoryJSONString(data, keys);

    // send json over network
    auto result = this->client_->send(jsonData);

    if(!result)
        logArgsError("client chat history sending error");

    return result;
}

bool Companion::sendFileRequest(std::shared_ptr<FileMessageWidget> widget)
{
    auto message = this->getMappedMessageByMessageWidget(true, widget);

    if(!message)
        return false;

    std::shared_ptr<MessageState> state = nullptr;

    {
        std::lock_guard<std::mutex> lock(this->messagesMutex_);

        try {
            state = this->messageMapping_.at(message)->getState();
        }
        catch(std::out_of_range) {}
    }

    if(!state)
        return false;

    bool result = this->sendMessage(
        false, NetworkMessageType::FILE_REQUEST, state->getNetworkId(), message);

    return result;
}

bool Companion::sendFileBlock(const std::string& networkId, const std::string& data)
{
    bool isConnected = this->client_->getIsConnected();

    if (!isConnected)
        return false;

    // build json
    auto jsonData = buildFileBlockJSONString(shared_from_this(), networkId, data);

    // send json over network
    bool result = this->client_->send(jsonData);

    if(!result)
        logArgsError("client message sending error");

    return result;
}

void Companion::updateData(std::shared_ptr<CompanionData> data)
{
    this->name_ = data->getName();
    this->socketInfo_->updateData(data);
}

std::shared_ptr<Message> Companion::findMessage(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto iter){
            return iter.first->getId() == messageId;
        });

    return (result == this->messageMapping_.end()) ? nullptr : result->first;
}

void Companion::addMessageWidgetsToChatHistory()
{
    auto widgetGroup = getManager()->getMappedWidgetGroupByCompanion(shared_from_this());

    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    for(auto& iterator : this->messageMapping_) {
        widgetGroup->addMessageWidgetToCentralPanelChatHistory(
            iterator.first, iterator.second->getState());
    }
}

void Companion::clearMessageMapping() {
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    this->messageMapping_.clear();
}

std::string Companion::generateNewNetworkId(bool lock) {
    if (lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    std::string networkId { "" };

    if (networkId.size() == 0) {  // TODO ???
        networkId = getRandomString(5);

        auto lambda = [&](){
            auto iterator = std::find_if(
                this->messageMapping_.begin(),
                this->messageMapping_.end(),
                [&](auto iter){
                    return iter.second->getState()->getNetworkId() == networkId;
                });

            return !(iterator == this->messageMapping_.end());
        };

        // loop while generated key is not unique
        while(lambda()) {
            networkId = getRandomString(5);
        }
    }

    return networkId;
}
