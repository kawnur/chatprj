#include "message_data.hpp"

#include "logging.hpp"
#include "priority_task_queue.hpp"

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

void MessageData::log()
{
    auto entryTemplate = "data: {0}, text: {1}"s;

    logTemplateInfo(entryTemplate, data_, text_);
}
