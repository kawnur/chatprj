#ifndef MESSAGE_STATE_HPP
#define MESSAGE_STATE_HPP

#include <nlohmann/json.hpp>

#include "alias.hpp"

class MessageState
{
public:
    MessageState() = default;
    MessageState(/*bool isAntecedent, */bool isSent, bool isReceived);
    MessageState(MessageStatePtr state);

    virtual ~MessageState() = default;

    // bool isAntecedent() const;
    bool isSent() const;
    bool isReceived() const;

    virtual bool isRead() { return false;}

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    // void setIsAntecedent(bool newIsAntecedent);
    void setIsSent(bool newIsSent);
    void setIsReceived(bool newIsReceived);

    void log();

    virtual nlohmann::json getJsonData() { return nlohmann::json::object(); }

protected:
    // bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
};

/*!
 * \brief The IncomingTextMessageState class - contains isRead field
 */
class IncomingTextMessageState : public MessageState
{
public:
    IncomingTextMessageState() = default;
    IncomingTextMessageState(bool isSent, bool isReceived, bool isRead);
    ~IncomingTextMessageState() = default;

    bool isRead() override;

    void setIsRead(bool newIsRead);

private:
    bool isRead_;
};

class ReceiveConfirmationMessageState : public MessageState
{
public:
    ReceiveConfirmationMessageState() = default;
    ReceiveConfirmationMessageState(/*bool isAntecedent, */bool isSent, bool isReceived);
    ~ReceiveConfirmationMessageState() = default;

    nlohmann::json getJsonData() override;
};

#endif // MESSAGE_STATE_HPP
