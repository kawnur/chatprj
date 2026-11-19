#include "message_widget_header.hpp"

#include "companion.hpp"
#include "message.hpp"
#include "utils_widgets.hpp"

MessageHeaderWidget::MessageHeaderWidget() : HLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft)
{
    label_ = std::make_shared<QLabel>();
    addWidget(label_.get());
}

void MessageHeaderWidget::setLabel(
    const std::string &color, const std::string &sender, const std::string &receiver,
    const std::string &time)
{
    auto text = getHTMLHeader(color, sender, receiver, time);
    label_->setText(text);
}

IncomingMessageHeaderWidget::IncomingMessageHeaderWidget(
    CompanionPtr companion, MessagePtr message)
    : MessageHeaderWidget()
{
    auto color = getHTMLColor(WidgetType::TEXT_RECEIVED);
    auto sender = companion->name();
    auto receiver = MESSAGE_WIDGET_ME_NAME;
    auto time = message->time();

    setLabel(color, sender, receiver, time);
}

OutgoingMessageHeaderWidget::OutgoingMessageHeaderWidget(
    CompanionPtr companion, MessagePtr message)
    : MessageHeaderWidget()
{
    auto color = getHTMLColor(WidgetType::TEXT_SENT);
    auto sender = MESSAGE_WIDGET_ME_NAME;
    auto receiver = companion->name();
    auto time = message->time();

    setLabel(color, sender, receiver, time);
}
