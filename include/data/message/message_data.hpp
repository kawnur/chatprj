#ifndef MESSAGE_DATA_HPP
#define MESSAGE_DATA_HPP

#include <string>

#include <nlohmann/json.hpp>

#include "alias.hpp"

class MessageData
{
public:
    MessageData() = default;
    MessageData(const std::string &data);
    MessageData(MessageDataPtr data);

    ~MessageData() = default;

    std::string data() const;

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    void setData(const std::string &newData);

    void log();

    virtual nlohmann::json getJsonData() { return nlohmann::json::object(); }

protected:
    std::string data_;
};

class TextMessageData : public MessageData
{
public:
    TextMessageData();
    TextMessageData(const std::string &data);
    ~TextMessageData() = default;

    nlohmann::json getJsonData() override;
};

#endif // MESSAGE_DATA_HPP
