#include "message_info.hpp"

MessageInfo::MessageInfo(MessagePtr message, MessageWidgetPtr widget)
    : message_(message), widget_(widget) {}

MessagePtr MessageInfo::message() const
{
    return message_;
}

MessageWidgetPtr MessageInfo::widget() const
{
    return widget_;
}

void MessageInfo::setWidget(MessageWidgetPtr widget)
{
    widget_ = widget;
}
