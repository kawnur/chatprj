#include "message_widget_indicator_panel.hpp"

#include "message.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget()
    : HLayoutWidget(WidgetType::MESSAGE_WIDGET_INDICATOR_PANEL, Qt::AlignRight)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

IncomingTextMessageIndicatorPanelWidget::IncomingTextMessageIndicatorPanelWidget(
    IncomingTextMessagePtr message)
    : MessageIndicatorPanelWidget()
{
    auto color = getHTMLColor(WidgetType::TEXT_RECEIVED);
    auto text = (message->isRead()) ? ""s : "NEW"s;
    auto textHtml = getHTMLBody(color, text, false, true);
    newMessageLabel_ = std::make_shared<QLabel>(textHtml);
    newMessageLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    addWidget(newMessageLabel_.get());
}

void IncomingTextMessageIndicatorPanelWidget::unsetNewMessageLabel()
{
    newMessageLabel_->setText("");
}

OutgoingTextMessageIndicatorPanelWidget::OutgoingTextMessageIndicatorPanelWidget(MessagePtr message)
    : MessageIndicatorPanelWidget()
{
    auto type = WidgetType::MESSAGE_WIDGET_INDICATOR;
    auto sentState = getIndicatorWidgetState(message->isSent());
    auto receivedState = getIndicatorWidgetState(message->isReceived());

    sent_ = std::make_shared<IndicatorWidget>(type, sentState);
    received_ = std::make_shared<IndicatorWidget>(type, receivedState);

    addWidget(sent_.get());
    addWidget(received_.get());
}

void OutgoingTextMessageIndicatorPanelWidget::setSentIndicatorOn()
{
    sent_->setOn();
}

void OutgoingTextMessageIndicatorPanelWidget::setReceivedIndicatorOn()
{
    received_->setOn();
}
