#include "companion.hpp"

SocketInfo::SocketInfo(
    std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    ipAddress_(ipAddress), serverPort_(serverPort), clientPort_(clientPort) {}

SocketInfo::SocketInfo(
    std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :  // TODO ???
    ipAddress_(ipAddress),
    serverPort_(serverPort),
    clientPort_(clientPort) {}

SocketInfo::SocketInfo(const SocketInfo& si)
{
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;
}

std::string SocketInfo::getIpAddress() const
{
    return this->ipAddress_;
}

uint16_t SocketInfo::getServerPort() const
{
    return this->serverPort_;
}

uint16_t SocketInfo::getClientPort() const
{
    return this->clientPort_;
}

void SocketInfo::updateData(const CompanionData* dataPtr)
{
    this->ipAddress_ = dataPtr->getIpAddress();
    this->clientPort_ = std::stoi(dataPtr->getClientPort());
}

Companion::Companion(int id, const std::string& name) :
    messagesMutex_(std::mutex()), id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr),
    messageMapping_(std::map<Message, MessageInfo>()) {}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;
}

int Companion::getId() const
{
    return this->id_;
}

std::string Companion::getName() const
{
    return this->name_;
}

SocketInfo* Companion::getSocketInfoPtr() const
{
    return this->socketInfoPtr_;
}

std::string Companion::getSocketIpAddress() const
{
    return this->socketInfoPtr_->getIpAddress();
}

uint16_t Companion::getSocketServerPort() const
{
    return this->socketInfoPtr_->getServerPort();
}

uint16_t Companion::getSocketClientPort() const
{
    return this->socketInfoPtr_->getClientPort();
}

const MessageState* Companion::getMappedMessageStatePtrByMessagePtr(
    const Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return &(iter.first) == messagePtr;
        });

    return (result == this->messageMapping_.end()) ? nullptr : result->second.getStatePtr();
}

MessageWidget* Companion::getMappedMessageWidgetPtrByMessagePtr(const Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return &(iter.first) == messagePtr;
        });

    return (result == this->messageMapping_.end()) ? nullptr : result->second.getWidgetPtr();
}

const Message* Companion::getMappedMessagePtrByMessageWidgetPtr(
    bool lock, MessageWidget* widgetPtr)
{
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return iter.second.getWidgetPtr() == widgetPtr;
        });

    return (result == this->messageMapping_.end()) ? nullptr : &(result->first);
}

MessageState* Companion::getMappedMessageStatePtrByMessageWidgetPtr(
    bool lock, MessageWidget* widgetPtr)
{
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    // TODO switch to map find method

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return iter.second.getWidgetPtr() == widgetPtr;
        });

    return (result == this->messageMapping_.end()) ?
        nullptr : result->second.getStatePtr();
}

std::pair<const Message, MessageInfo>* Companion::getMessageMappingPairPtrByMessageId(
    uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return iter.first.getId() == messageId;
        });

    return (result == this->messageMapping_.end()) ? nullptr : &(*result);
}

std::pair<const Message, MessageInfo>* Companion::getMessageMappingPairPtrByNetworkId(
    const std::string& networkId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return iter.second.getStatePtr()->getNetworkId() == networkId;
        });

    return (result == this->messageMapping_.end()) ? nullptr : &(*result);
}

const Message* Companion::getEarliestMessagePtr() const
{
    auto minPair = std::min_element(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iterator1, auto& iterator2)
        {
            return iterator1.first.getId() < iterator2.first.getId();
        });

    return &(minPair->first);
}

std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
Companion::createMessageAndAddToMapping(
    uint32_t messageId, uint8_t authorId,
    const std::string& messageTime, const std::string& messageText,
    bool isAntecedent, bool isSent, bool isReceived, std::string networkId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    if(networkId.size() == 0)
    {
        networkId = this->generateNewNetworkId(false);
    }

    auto companionId = this->getId();

    MessageState* messageStatePtr = new MessageState(
        companionId, isAntecedent, isSent, isReceived, networkId);

    auto result = this->messageMapping_.emplace(
        std::make_pair(
            Message(messageId, companionId, authorId, messageTime, messageText),
            MessageInfo(messageStatePtr, nullptr)));

    return result;
}

std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
Companion::createMessageAndAddToMapping(
    std::shared_ptr<DBReplyData>& messagesDataPtr, size_t index)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto id = this->getId();

    MessageState* messageStatePtr = new MessageState(
        id, false,
        getBoolFromDBValue(messagesDataPtr->getValue(index, "is_sent")),
        getBoolFromDBValue(messagesDataPtr->getValue(index, "is_received")),
        this->generateNewNetworkId(false));

    auto result = this->messageMapping_.emplace(
        std::make_pair(
            Message(
                std::atoi(messagesDataPtr->getValue(index, "id")),
                id,
                std::atoi(messagesDataPtr->getValue(index, "author_id")),
                messagesDataPtr->getValue(index, "timestamp_tz"),
                messagesDataPtr->getValue(index, "message")
            ),
            MessageInfo(messageStatePtr, nullptr)
        )
    );

    return result;
}

void Companion::setSocketInfo(SocketInfo* socketInfo)
{
    socketInfoPtr_ = socketInfo;
}

void Companion::setMappedMessageWidget(const Message* messagePtr, MessageWidget* widgetPtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = this->messageMapping_.find(*messagePtr);

    if(result == this->messageMapping_.end())
    {
        logArgsError(
            nullptr,
            QString("message with id %1 was not found in messageMapping_")
                .arg(messagePtr->getId()));
    }
    else
    {
        result->second.setWidgetPtr(widgetPtr);
    }
}

bool Companion::startServer()
{
    bool started = false;

    auto startLambda = [this](bool& value)
    {
        this->serverPtr_ = new ChatServer(this, this->socketInfoPtr_->getServerPort());
        this->serverPtr_->run();
        value = true;
    };

    runAndLogException(startLambda, started);

    return started;
}

bool Companion::createClient()
{
    bool created = false;

    auto createLambda = [this](bool& value)
    {
        this->clientPtr_ = new ChatClient(
            this->socketInfoPtr_->getIpAddress(),
            this->socketInfoPtr_->getClientPort());

        value = true;
    };

    runAndLogException(createLambda, created);

    return created;
}

bool Companion::connectClient()
{
    return this->clientPtr_->connect();
}

bool Companion::disconnectClient()
{
    return this->clientPtr_->disconnect();
}

bool Companion::sendMessage(
    bool isAntecedent, NetworkMessageType type,
    std::string networkId, const Message* messagePtr)
{    
    if(this->clientPtr_)
    {
        bool isConnected = this->clientPtr_->getIsConnected();

        logArgs("clientPtr_->getIsConnected():", isConnected);

        if(isConnected)
        {
            // build json
            std::string jsonData = buildMessageJSONString(
                isAntecedent, type, this, networkId, messagePtr);

            // send json over network
            auto result = this->clientPtr_->send(jsonData);

            if(!result)
            {
                logArgsError("client message sending error");
            }

            return result;
        }
    }

    return false;
}

bool Companion::sendChatHistory(
    std::shared_ptr<DBReplyData>& dataPtr, std::vector<std::string>& keys) const
{
    if(this->clientPtr_)
    {
        bool isConnected = this->clientPtr_->getIsConnected();

        logArgs("clientPtr_->getIsConnected():", isConnected);

        if(isConnected)
        {
            // build json
            std::string jsonData = buildChatHistoryJSONString(dataPtr, keys);

            // send json over network
            auto result = this->clientPtr_->send(jsonData);

            if(!result)
            {
                logArgsError("client chat history sending error");
            }

            return result;
        }
    }

    return false;
}

void Companion::updateData(const CompanionData* dataPtr)
{
    this->name_ = dataPtr->getName();
    this->socketInfoPtr_->updateData(dataPtr);
}

Message* Companion::findMessage(uint32_t messageId)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto iter)
        {
            return iter.first.getId() == messageId;
        });

    return (result == this->messageMapping_.end()) ?
               nullptr : const_cast<Message*>(&(result->first));
}

void Companion::addMessageWidgetsToChatHistory()
{
    WidgetGroup* widgetGroupPtr =
        getManagerPtr()->getMappedWidgetGroupPtrByCompanionPtr(this);

    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    logArgsWithCustomMark(
        "this->messageMapping_.size():", this->messageMapping_.size());

    for(auto& iterator : this->messageMapping_)
    {
        widgetGroupPtr->addMessageWidgetToCentralPanelChatHistory(
            &(iterator.first), iterator.second.getStatePtr());
    }
}

void Companion::clearMessageMapping()
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    this->messageMapping_.clear();
}

std::string Companion::generateNewNetworkId(bool lock)
{
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->messagesMutex_);

    std::string networkId { "" };

    if(networkId.size() == 0)
    {
        networkId = getRandomString(5);

        auto lambda = [&]()
        {
            auto iterator = std::find_if(
                this->messageMapping_.begin(),
                this->messageMapping_.end(),
                [&](auto iter)
                {
                    return iter.second.getStatePtr()->getNetworkId() == networkId;
                });

            return !(iterator == this->messageMapping_.end());
        };

        // loop while generated key is not unique
        while(lambda())
        {
            networkId = getRandomString(5);
        }
    }

    return networkId;
}
