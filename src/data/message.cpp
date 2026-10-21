#include "message.hpp"

#include "companion.hpp"
#include "constants.hpp"
#include "logging.hpp"

PriorityTask::PriorityTask(
    uint8_t priority, const std::string &key, std::function<void ()> function, bool isMandatory)
    : priority_(priority), isMandatory_(isMandatory), key_(key), func_(function) {}

PriorityTask::~PriorityTask() { logArgsInfo(Q_FUNC_INFO, key_); }

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
        logArgsError("MessageMetaData is invalid, error adding message to db");

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

MessageData::MessageData(const std::string &text, const std::string &data)
    : text_(text), data_(data) {}

// Message::Message(
//     MessageType type, /*uint32_t id, uint8_t companion_id, uint8_t author_id, const std::string &time,*/
//      const MessageMetaData &meta, const std::string &text)
//     // : type_(type), id_(id), companion_id_(companion_id), author_id_(author_id), time_(time),
//     // text_(text) {}
// {
//     type_ = type;
//     id_ = meta.messageId_;
//     companion_id_ = meta.companionId_;
//     author_id_ = meta.authorId_;
//     time_ = meta.timestampTz_;
//     text_ = text;
// }

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

Message::Message(
    MessageMetaDataPtr meta, MessageDataPtr data,
    MessageStatePtr state) : meta_(meta), data_(data), state_(state) {}

Message::~Message() = default;

MessageType Message::type() const
{
    return meta_->messageType_;
}

uint32_t Message::id() const
{
    // return id_;
    return meta_->messageId_;
}

uint8_t Message::companionId() const
{
    return meta_->companionId_;
}

std::string Message::companionName() const
{
    return meta_->companionName_;
}

uint8_t Message::authorId() const
{
    return meta_->authorId_;
}

std::string Message::authorName() const
{
    return meta_->authorName_;
}

std::string Message::time() const
{
    return meta_->timestampTz_;
}

std::string Message::text() const
{
    return data_->text_;
}

bool Message::isMessageFromMe() const
{
    return (meta_->authorId_ != meta_->companionId_);
}

bool Message::isSent() const
{
    return state_->isSent_;
}

bool Message::isReceived() const
{
    return state_->isReceived_;
}

bool Message::isAntecedent() const
{
    return state_->isAntecedent_;
}

std::string Message::networkId() const
{
    return meta_->networkId_;
}

NetworkMessageType Message::networkMessageType() const
{
    return meta_->networkMessageType_;
}

void Message::setId(int id)
{
    meta_->messageId_ = id;
}

void Message::setCompanionId(int id)
{
    meta_->companionId_ = id;
}

void Message::setTime(const std::string &time)
{
    meta_->timestampTz_ = time;
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

// bool MessageState::isAntecedent() const
// {
//     return isAntecedent_;
// }

// bool MessageState::isSent() const
// {
//     return isSent_;
// }

// bool MessageState::isReceived() const
// {
//     return isReceived_;
// }

// std::string MessageState::getNetworkId() const
// {
//     return networkId_;
// }

// void MessageState::setIsAntecedent(bool value)
// {
//     isAntecedent_ = value;
// }

// void MessageState::setIsReceived(bool value)
// {
//     isReceived_ = value;
// }

// void MessageState::setNetworkId(const std::string &networkId)
// {
//     networkId_ = networkId;
// }

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
