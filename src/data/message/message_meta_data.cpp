#include "message_meta_data.hpp"

#include "companion.hpp"
#include "logging.hpp"
#include "priority_task_queue.hpp"

MessageMetaData::MessageMetaData(
    uint32_t messageId, uint8_t companionId, uint8_t authorId, const std::string &timestampTz,
    const std::string networkId)
    : messageId_(messageId), companionId_(companionId), authorId_(authorId),
    timestampTz_(timestampTz), networkId_(networkId) {}

MessageMetaData::MessageMetaData(MessageMetaDataPtr meta)
{
    messageId_ = meta->messageId_;
    companionId_ = meta->companionId_;
    authorId_ = meta->authorId_;
    timestampTz_ = meta->timestampTz_;
    networkId_ = meta->networkId_;
}

bool MessageMetaData::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 1, "companion_id", [&]() { companionId_ = companion->id(); } },
            {
                0, "author_id",
                [&]()
                {
                    authorId_ = companionId_;
                }
            },
            { 0, "time", [&]() { timestampTz_ = data.at("time"); } },
            { 0, "id", [&]() { networkId_ = data.at("id"); } },
            // { 0, "hashMD5", [&]() { hashMD5_ = data.at("hashMD5"); }, false }
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

uint32_t MessageMetaData::messageId() const
{
    return messageId_;
}

uint8_t MessageMetaData::companionId() const
{
    return companionId_;
}

uint8_t MessageMetaData::authorId() const
{
    return authorId_;
}

std::string MessageMetaData::timestampTz() const
{
    return timestampTz_;
}

// std::string MessageMetaData::hashMD5() const
// {
//     return hashMD5_;
// }

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

std::string MessageMetaData::getLogString()
{
    auto entryTemplate =
        "messageId: {0}, companionId: {1}, authorId: {2}, timestampTz: {3}, networkId: {4}"s;

    return getStringByFormat(
        entryTemplate, messageId_, companionId_, authorId_, timestampTz_, networkId_);
}
void MessageMetaData::log()
{
    logTemplateInfo(getLogString());
}

nlohmann::json MessageMetaData::getJsonData()
{
    nlohmann::json data;

    data["id"] = networkId_;
    data["time"] = timestampTz_;

    return data;
}
