#include "message.hpp"

#include "companion.hpp"
#include "constants.hpp"
#include "logging.hpp"

PriorityTask::PriorityTask(
    uint8_t priority, const std::string &key, std::function<void ()> function, bool isMandatory)
    : priority_(priority), isMandatory_(isMandatory), key_(key), func_(function) {}

PriorityTask::~PriorityTask() { /*logArgsInfo(Q_FUNC_INFO, key_);*/ }

uint8_t PriorityTask::priority() const
{
    return priority_;
}

std::string PriorityTask::key() const
{
    return key_;
}

std::function<void ()> PriorityTask::func() const
{
    return func_;
}

bool PriorityTask::isMandatory() const
{
    return isMandatory_;
}

PriorityTaskQueueWrapper::PriorityTaskQueueWrapper(const std::vector<PriorityTask> &tasks)
{
    auto comparator = [](const auto &item1, const auto &item2)
    {
        return item1.priority() < item2.priority();
    };

    queue_ = PriorityTaskQueue(comparator, tasks);
}

PriorityTaskQueueWrapper::~PriorityTaskQueueWrapper() { logArgsInfo(Q_FUNC_INFO); }

bool PriorityTaskQueueWrapper::runTasksAndReturnResult()
{
    bool result = true;

    while (!queue_.empty()) {
        auto item = queue_.top();
        auto func = item.func();
        bool itemResult = (item.isMandatory()) ? runAndReturnBool(func) : runAndReturnTrue(func);
        result &= itemResult;
        queue_.pop();
    }

    return result;
}

MessageMetaData::MessageMetaData(
    MessageType messageType, NetworkMessageType networkMessageType, uint32_t messageId,
    uint8_t companionId, const std::string &companionName, uint8_t authorId,
    const std::string &authorName, const std::string &timestampTz, const std::string &hashMD5,
    const std::string networkId)
    : messageType_(messageType), networkMessageType_(networkMessageType), messageId_(messageId),
    companionId_(companionId), companionName_(companionName), authorId_(authorId),
    authorName_(authorName), timestampTz_(timestampTz), hashMD5_(hashMD5), networkId_(networkId) {}

MessageMetaData::MessageMetaData(MessageMetaDataPtr meta)
{
    messageType_ = meta->messageType_;
    networkMessageType_ = meta->networkMessageType_;
    messageId_ = meta->messageId_;
    companionId_ = meta->companionId_;
    companionName_ = meta->companionName_;
    authorId_ = meta->authorId_;
    authorName_ = meta->authorName_;
    timestampTz_ = meta->timestampTz_;
    hashMD5_ = meta->hashMD5_;
    networkId_ = meta->networkId_;
}

bool MessageMetaData::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 1, "type", [&]() { networkMessageType_ = data.at("type"); } },
            { 1, "companion_id", [&]() { companionId_ = companion->id(); } },
            {
                0, "author_id",
                [&]()
                {
                    if (networkMessageType_ == NetworkMessageType::TEXT)
                        authorId_ = companionId_;
                }
            },
            {
                0, "messageType",
                [&]()
                {
                    messageType_ = getMapValue(
                        NETWORK_MESSAGE_TYPE_TO_MESSAGE_TYPE_MAP, networkMessageType_,
                        MessageType::UNKNOWN);
                }
            },
            { 0, "time", [&]() { timestampTz_ = data.at("time"); } },
            { 0, "id", [&]() { networkId_ = data.at("id"); } },
            { 0, "hashMD5", [&]() { hashMD5_ = data.at("hashMD5"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

bool MessageMetaData::isValid()
{
    if (companionId_ == 0 || timestampTz_ == "") {
        logArgsError("MessageMetaData is invalid, adding message to db error");

        return false;
    }

    return true;
}

void MessageMetaData::update(MessageMetaDataPtr meta)
{
    messageId_ = meta->messageId_;
    companionId_ = meta->companionId_;
    timestampTz_ = meta->timestampTz_;
}

bool MessageMetaData::isMessageFromMe()
{
    return (authorId_ == 1);
}

MessageType MessageMetaData::messageType() const
{
    return messageType_;
}

NetworkMessageType MessageMetaData::networkMessageType() const
{
    return networkMessageType_;
}

uint32_t MessageMetaData::messageId() const
{
    return messageId_;
}

uint8_t MessageMetaData::companionId() const
{
    return companionId_;
}

std::string MessageMetaData::companionName() const
{
    return companionName_;
}

uint8_t MessageMetaData::authorId() const
{
    return authorId_;
}

std::string MessageMetaData::authorName() const
{
    return authorName_;
}

std::string MessageMetaData::timestampTz() const
{
    return timestampTz_;
}

std::string MessageMetaData::hashMD5() const
{
    return hashMD5_;
}

std::string MessageMetaData::networkId() const
{
    return networkId_;
}

void MessageMetaData::setMessageId(uint32_t newMessageId)
{
    messageId_ = newMessageId;
}

void MessageMetaData::setCompanionId(uint8_t newCompanionId)
{
    companionId_ = newCompanionId;
}

void MessageMetaData::setTimestampTz(const std::string &newTimestampTz)
{
    timestampTz_ = newTimestampTz;
}

void MessageMetaData::setNetworkId(const std::string &newNetworkId)
{
    networkId_ = newNetworkId;
}

void MessageMetaData::setNetworkMessageType(NetworkMessageType newNetworkMessageType)
{
    networkMessageType_ = newNetworkMessageType;
}

void MessageMetaData::setMessageType(MessageType newMessageType)
{
    messageType_ = newMessageType;
}

void MessageMetaData::setCompanionName(const std::string &newCompanionName)
{
    companionName_ = newCompanionName;
}

void MessageMetaData::setAuthorName(const std::string &newAuthorName)
{
    authorName_ = newAuthorName;
}

MessageData::MessageData(const std::string &text, const std::string &data)
    : text_(text), data_(data) {}

MessageData::MessageData(MessageDataPtr data)
{
    data_ = data->data_;
    text_ = data->text_;
}

bool MessageData::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 0, "text", [&]() { text_ = data.at("text"); } },
            { 0, "data", [&]() { data_ = data.at("data"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

std::string MessageData::text() const
{
    return text_;
}

void MessageData::setText(const std::string &newText)
{
    text_ = newText;
}

std::string MessageData::data() const
{
    return data_;
}

void MessageData::setData(const std::string &newData)
{
    data_ = newData;
}

Message::Message(
    MessageMetaDataPtr meta, MessageDataPtr data,
    MessageStatePtr state) : meta_(meta), data_(data), state_(state) {}

Message::Message(MessagePtr message)
{
    meta_ = message->meta();
    data_ = message->data();
    state_ = message->state();
}

Message::~Message() = default;

MessageType Message::type() const
{
    return meta_->messageType();
}

uint32_t Message::id() const
{
    // return id_;
    return meta_->messageId();
}

uint8_t Message::companionId() const
{
    return meta_->companionId();
}

std::string Message::companionName() const
{
    return meta_->companionName();
}

uint8_t Message::authorId() const
{
    return meta_->authorId();
}

std::string Message::authorName() const
{
    return meta_->authorName();
}

std::string Message::time() const
{
    return meta_->timestampTz();
}

std::string Message::hashMD5() const
{
    return meta_->hashMD5();
}

std::string Message::text() const
{
    return data_->text();
}

bool Message::isMessageFromMe() const
{
    return meta_->isMessageFromMe();
}

bool Message::isSent() const
{
    return state_->isSent();
}

bool Message::isReceived() const
{
    return state_->isReceived();
}

bool Message::isAntecedent() const
{
    return state_->isAntecedent();
}

std::string Message::networkId() const
{
    return meta_->networkId();
}

NetworkMessageType Message::networkMessageType() const
{
    return meta_->networkMessageType();
}

void Message::setId(int id)
{
    meta_->setMessageId(id);
}

void Message::setCompanionId(int id)
{
    meta_->setCompanionId(id);
}

void Message::setTime(const std::string &time)
{
    meta_->setTimestampTz(time);
}

MessageMetaDataPtr Message::meta() const
{
    return meta_;
}

MessageDataPtr Message::data() const
{
    return data_;
}

MessageStatePtr Message::state() const
{
    return state_;
}

void Message::updateMeta(MessageMetaDataPtr meta)
{
    meta_->update(meta);
}

// MessageState::MessageState(
//     uint8_t companionId, bool isAntecedent, bool isSent, bool isReceived, std::string networkId)
//     : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived), networkId_(networkId)
// {}

MessageState::MessageState(bool isAntecedent, bool isSent, bool isReceived)
    : isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived) {}

MessageState::MessageState(MessageStatePtr state)
{
    isAntecedent_ = state->isAntecedent_;
    isSent_ = state->isSent_;
    isReceived_ = state->isReceived_;
}

bool MessageState::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 0, "antecedent", [&]() { isAntecedent_ = data.at("antecedent"); } },
            { 0, "received", [&]() { isReceived_ = data.at("received"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

bool MessageState::isAntecedent() const
{
    return isAntecedent_;
}

void MessageState::setIsAntecedent(bool newIsAntecedent)
{
    isAntecedent_ = newIsAntecedent;
}

bool MessageState::isSent() const
{
    return isSent_;
}

void MessageState::setIsSent(bool newIsSent)
{
    isSent_ = newIsSent;
}

bool MessageState::isReceived() const
{
    return isReceived_;
}

void MessageState::setIsReceived(bool newIsReceived)
{
    isReceived_ = newIsReceived;
}

MessageInfo::MessageInfo(MessagePtr message, MessageWidgetPtr widget)
    : message_(message), widget_(widget) {}

MessagePtr MessageInfo::message() const
{
    return message_;
}

MessageWidgetPtr MessageInfo::widget() const
{
    return widget_;
}

void MessageInfo::setWidget(MessageWidgetPtr widget)
{
    widget_ = widget;
}

bool operator<(const Message &message1, const Message &message2)
{
    return message1.id() < message2.id();
}
