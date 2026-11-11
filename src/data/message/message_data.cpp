#include "message_data.hpp"

#include "logging.hpp"
#include "priority_task_queue.hpp"

MessageData::MessageData(const std::string &data) : data_(data) {}

MessageData::MessageData(MessageDataPtr data)
{
    data_ = data->data_;
}

bool MessageData::setFields(CompanionPtr companion, const nlohmann::json &data)
{
    std::vector<PriorityTask> taskVector
        {
            { 0, "text", [&]() { data_ = data.at("text"); }, false }
        };

    PriorityTaskQueueWrapper wrapper(taskVector);
    bool result = wrapper.runTasksAndReturnResult();

    return result;
}

std::string MessageData::data() const
{
    return data_;
}

void MessageData::setData(const std::string &newData)
{
    data_ = newData;
}

void MessageData::log()
{
    auto entryTemplate = "data: {0}"s;

    logTemplateInfo(entryTemplate, data_);
}

TextMessageData::TextMessageData() : MessageData() {}

TextMessageData::TextMessageData(const std::string &data) : MessageData(data) {}

nlohmann::json TextMessageData::getJsonData()
{
    nlohmann::json data;

    data["text"] = data_;

    return data;
}
