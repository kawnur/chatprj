#ifndef MESSAGE_STATE_HPP
#define MESSAGE_STATE_HPP

#include <nlohmann/json.hpp>

#include "alias.hpp"

class MessageState
{
public:
    MessageState() = default;
    MessageState(bool isAntecedent, bool isSent, bool isReceived);
    MessageState(MessageStatePtr state);

    ~MessageState() = default;

    bool isAntecedent() const;
    bool isSent() const;
    bool isReceived() const;

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    void setIsAntecedent(bool newIsAntecedent);
    void setIsSent(bool newIsSent);
    void setIsReceived(bool newIsReceived);

    void log();

private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
};

#endif // MESSAGE_STATE_HPP
