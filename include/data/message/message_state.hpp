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

    virtual nlohmann::json getJsonData() { return nlohmann::json::object(); }

protected:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
};

class ReceiveConfirmationMessageState : public MessageState
{
public:
    ReceiveConfirmationMessageState() = default;
    ReceiveConfirmationMessageState(bool isAntecedent, bool isSent, bool isReceived);
    ~ReceiveConfirmationMessageState() = default;

    nlohmann::json getJsonData() override;
};

#endif // MESSAGE_STATE_HPP
