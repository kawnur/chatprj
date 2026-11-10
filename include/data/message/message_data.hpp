#ifndef MESSAGE_DATA_HPP
#define MESSAGE_DATA_HPP

#include <string>

#include <nlohmann/json.hpp>

#include "alias.hpp"

class MessageData
{
public:
    MessageData() = default;
    MessageData(const std::string &text, const std::string &data);
    MessageData(MessageDataPtr data);

    ~MessageData() = default;

    std::string text() const;
    std::string data() const;

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    void setText(const std::string &newText);
    void setData(const std::string &newData);

    void log();

private:
    std::string text_;
    std::string data_;  // for file transfer
};

#endif // MESSAGE_DATA_HPP
