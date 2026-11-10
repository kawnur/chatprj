#ifndef MESSAGE_INFO_HPP
#define MESSAGE_INFO_HPP

#include "alias.hpp"

class MessageInfo
{
public:
    MessageInfo(MessagePtr message, MessageWidgetPtr widget);
    ~MessageInfo() = default;

    MessagePtr message() const;
    MessageWidgetPtr widget() const;

    void setWidget(MessageWidgetPtr widget);

private:
    MessagePtr message_;
    MessageWidgetPtr widget_;
};

#endif // MESSAGE_INFO_HPP
