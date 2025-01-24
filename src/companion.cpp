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
    // messagePointersPtr_(new std::vector<Message*>) {}
    messageMapping_(std::map<Message, MessageInfo>()) {}

// Companion::Companion(int id, std::string&& name) :
//     messagesMutex_(std::mutex()), id_(id), name_(name), socketInfoPtr_(nullptr),
//     clientPtr_(nullptr), serverPtr_(nullptr),
//     // messagePointersPtr_(new std::vector<Message*>) {}
//     messageMapping_(std::map<Message, MessageInfo>()) {}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;

    // for(auto& messagePtr : *this->messagePointersPtr_)
    // {
    //     delete messagePtr;
    // }

    // delete this->messagePointersPtr_;
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

// const std::vector<Message*>* Companion::getMessagePointersPtr() const
// {
//     return this->messagePointersPtr_;
// }

const MessageState* Companion::getMappedMessageStateByMessagePtr(
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

MessageWidget* Companion::getMappedMessageWidgetByMessagePtr(const Message* messagePtr)
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

const Message* Companion::getMappedMessageByMessageWidgetPtr(MessageWidget* widgetPtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

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

std::pair<const Message, MessageInfo>* Companion::getMessageMappingPairPtrByMessageMappingKey(
    const std::string& key)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto& iter)
        {
            return iter.second.getStatePtr()->getMessageMappingKey() == key;
        });

    return (result == this->messageMapping_.end()) ? nullptr : &(*result);
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

// std::pair<std::iterator<std::contiguous_iterator_tag, std::pair<Message, MessageInfo>>, bool>
std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
Companion::createMessageAndAddToMapping(
    uint32_t messageId, uint8_t authorId,
    const std::string& messageTime, const std::string& messageText,
    bool isAntecedent, bool isSent, bool isReceived, std::string networkId)
{
    auto companionId = this->getId();

    MessageState* messageStatePtr = new MessageState(
        companionId, isAntecedent, isSent, isReceived, networkId);

    auto result = this->messageMapping_.emplace(
        std::make_pair(
            Message(messageId, companionId, authorId, messageTime, messageText),
            MessageInfo(messageStatePtr, nullptr)
            )
        );

    return result;
}

// std::pair<std::iterator<std::contiguous_iterator_tag, std::pair<Message, MessageInfo>>, bool>
std::pair<std::_Rb_tree_iterator<std::pair<const Message, MessageInfo>>, bool>
Companion::createMessageAndAddToMapping(
    std::shared_ptr<DBReplyData>& messagesDataPtr, size_t index)
{
    auto id = this->getId();

    // Message* messagePtr = new Message(
    //     std::atoi(messagesDataPtr->getValue(index, "id")),
    //     id,
    //     std::atoi(messagesDataPtr->getValue(index, "author_id")),
    //     messagesDataPtr->getValue(index, "timestamp_tz"),
    //     messagesDataPtr->getValue(index, "message"));

    // companionPtr->addMessage(messagePtr);

    MessageState* messageStatePtr = new MessageState(
        id, false,
        messagesDataPtr->getValue(index, "is_sent"),
        messagesDataPtr->getValue(index, "is_received"), "");

    // this->addToMessageStateToMessageMapping(messageStatePtr, messagePtr);

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

    // std::find_if(
    //     this->messageMapping_.begin(),
    //     this->messageMapping_.end(),
    //     [&](auto& iter)
    //     {
    //         return iter.first.getId() == messageId;
    //     });
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

// void Companion::clearMessages()
// {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);
//     this->messagePointersPtr_->clear();
// }

// void Companion::addMessage(Message* messagePtr)
// {
//     std::lock_guard<std::mutex> lock(this->messagesMutex_);
//     this->messagePointersPtr_->push_back(messagePtr);
// }

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
                isAntecedent, type, networkId, messagePtr);

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

    // auto result = std::find_if(
    //     this->messagePointersPtr_->begin(),
    //     this->messagePointersPtr_->end(),
    //     [&](auto iter)
    //     {
    //         return iter->getId() == messageId;
    //     });

    auto result = std::find_if(
        this->messageMapping_.begin(),
        this->messageMapping_.end(),
        [&](auto iter)
        {
            return iter.first.getId() == messageId;
        });

    // return (result == this->messagePointersPtr_->end()) ? nullptr : *result;
    return (result == this->messageMapping_.end()) ?
               nullptr : const_cast<Message*>(&(result->first));
}

void Companion::addMessageWidgetsToChatHistory(
    const WidgetGroup* widgetGroupPtr, CentralPanelWidget* centralPanelWidgetPtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);

    for(auto& iterator : this->messageMapping_)
    {
        centralPanelWidgetPtr->addMessageWidgetToChatHistory(
            widgetGroupPtr, this, &(iterator.first), iterator.second.getStatePtr());
    }
}
