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

void SocketInfo::print()
{
    logArgs("ipAddress:", this->ipAddress_);
    logArgs("serverPort_:", this->serverPort_);
    logArgs("clientPort_:", this->clientPort_);
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
    messagePointersPtr_(new std::vector<Message*>) {}

Companion::Companion(int id, std::string&& name) :
    messagesMutex_(std::mutex()), id_(id), name_(name), socketInfoPtr_(nullptr),
    clientPtr_(nullptr), serverPtr_(nullptr),
    messagePointersPtr_(new std::vector<Message*>) {}

Companion::~Companion()
{
    delete socketInfoPtr_;
    delete clientPtr_;
    delete serverPtr_;

    for(auto& messagePtr : *this->messagePointersPtr_)
    {
        delete messagePtr;
    }

    delete this->messagePointersPtr_;
}

bool Companion::startServer()
{
    bool started = false;

    auto startLambda = [this](bool& value)
    {
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

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
        // io_contextPtr_ = new boost::asio::io_context;

        // serverPtr_ = new ChatServer(
        //     *io_contextPtr_,
        //     this->socketInfoPtr_->getPortInt());

        this->clientPtr_ = new ChatClient(
            this->socketInfoPtr_->getIpAddress(),
            this->socketInfoPtr_->getClientPort());

        // created = this->clientPtr_->connect();
        value = true;
    };

    runAndLogException(createLambda, created);

    return created;
}

// bool Companion::connectClient()
// {
//     bool connected = false;

//     auto connectLambda = [this](bool& value)
//     {
//         // io_contextPtr_ = new boost::asio::io_context;

//         // serverPtr_ = new ChatServer(
//         //     *io_contextPtr_,
//         //     this->socketInfoPtr_->getPortInt());

//         // this->clientPtr_ = new ChatClient(
//         //     this->socketInfoPtr_->getIpAddress(),
//         //     this->socketInfoPtr_->getClientPort());

//         value = this->clientPtr_->connect();
//     };

//     runAndLogException(connectLambda, connected);

//     return connected;
// }

// bool Companion::disconnectClient()
// {
//     bool disconnected = false;

//     auto disconnectLambda = [this](bool& value)
//     {
//         // io_contextPtr_ = new boost::asio::io_context;

//         // serverPtr_ = new ChatServer(
//         //     *io_contextPtr_,
//         //     this->socketInfoPtr_->getPortInt());

//         // this->clientPtr_ = new ChatClient(
//         //     this->socketInfoPtr_->getIpAddress(),
//         //     this->socketInfoPtr_->getClientPort());

//         value = this->clientPtr_->disconnect();
//     };

//     runAndLogException(disconnectLambda, disconnected);

//     return disconnected;
// }

bool Companion::connectClient()
{
    return this->clientPtr_->connect();
}

bool Companion::disconnectClient()
{
    return this->clientPtr_->disconnect();
}

void Companion::setSocketInfo(SocketInfo* socketInfo)
{
    socketInfoPtr_ = socketInfo;
}

void Companion::addMessage(Message* messagePtr)
{
    std::lock_guard<std::mutex> lock(this->messagesMutex_);
    this->messagePointersPtr_->push_back(messagePtr);
}

const std::vector<Message*>* Companion::getMessagesPtr() const
{
    return this->messagePointersPtr_;
}

bool Companion::sendMessage(
    bool isAntecedent, NetworkMessageType type,
    std::string networkId, const Message* messagePtr)
{
    if(this->clientPtr_)
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
        this->messagePointersPtr_->begin(),
        this->messagePointersPtr_->end(),
        [&](auto iter)
        {
            return iter->getId() == messageId;
        });

    return (result == this->messagePointersPtr_->end()) ? nullptr : *result;
}

int Companion::getId()
{
    return this->id_;
}

std::string Companion::getName() const
{
    return this->name_;
}

std::string Companion::getIpAddress() const
{
    return this->socketInfoPtr_->getIpAddress();
}

uint16_t Companion::getServerPort() const
{
    return this->socketInfoPtr_->getServerPort();
}

uint16_t Companion::getClientPort() const
{
    return this->socketInfoPtr_->getClientPort();
}

SocketInfo* Companion::getSocketInfoPtr() const
{
    return this->socketInfoPtr_;
}
