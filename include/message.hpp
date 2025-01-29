#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string>

#include "utils.hpp"

class MessageWidget;

class Message
{
public:
    Message(uint32_t, uint8_t, uint8_t, const std::string&, const std::string&);
    ~Message() = default;

    uint32_t getId() const;
    uint8_t getCompanionId() const;
    uint8_t getAuthorId() const;
    std::string getTime() const;
    std::string getText() const;
    bool isMessageFromMe() const;

    friend bool operator<(const Message& message1, const Message& message2)
    {
        return message1.getId() < message2.getId();
    }

private:
    uint32_t id_;
    uint8_t companion_id_;
    uint8_t author_id_;
    std::string time_;  // TODO add timezone support
    std::string text_;
};

class MessageState
{
public:
    MessageState(uint8_t, bool, bool, bool, std::string);
    ~MessageState() = default;

    bool getIsAntecedent() const;
    bool getIsSent() const;
    bool getIsReceived() const;
    std::string getNetworkId() const;

    void setIsAntecedent(bool);
    void setIsReceived(bool);
    void setNetworkId(const std::string&);

private:
    bool isAntecedent_;
    bool isSent_;
    bool isReceived_;
    std::string networkId_;
};

class MessageInfo
{
public:
    MessageInfo(MessageState*, MessageWidget*);
    ~MessageInfo() = default;

    MessageState* getStatePtr() const;
    MessageWidget* getWidgetPtr() const;

    void setWidgetPtr(MessageWidget*);

private:
    MessageState* statePtr_;
    MessageWidget* widgetPtr_;
};

#endif // MESSAGE_HPP
