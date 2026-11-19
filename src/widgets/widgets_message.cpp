#include "widgets_message.hpp"

#include "companion.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widget_group.hpp"

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

MessageBodyWidget::MessageBodyWidget() : HLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft) {}

TextMessageBodyWidget::TextMessageBodyWidget(WidgetType type, MessagePtr message)
    : MessageBodyWidget()
{
    setType(type);
    auto text = message->_data();
    auto color = getHTMLColor(type);
    auto body = getHTMLBody(color, text, true, false);
    label_ = std::make_shared<QLabel>(body);
    addWidget(label_.get());
}

IncomingTextMessageBodyWidget::IncomingTextMessageBodyWidget(MessagePtr message)
    : TextMessageBodyWidget(WidgetType::TEXT_RECEIVED, message) {}

OutgoingTextMessageBodyWidget::OutgoingTextMessageBodyWidget(MessagePtr message)
    : TextMessageBodyWidget(WidgetType::TEXT_SENT, message) {}

FileMessageBodyWidget::FileMessageBodyWidget(WidgetType type, MessagePtr message) : MessageBodyWidget()
{
    // type
    setType(type);

    // label
    label_ = std::make_shared<QLabel>();
    addWidget(label_.get());
}

void FileMessageBodyWidget::buildText(MessagePtr message)
{
    auto _type = type();
    auto color = getHTMLColor(_type);
    auto textHtml = getHTMLBody(color, getText(message), true, false);
    label_->setText(textHtml);
}

IncomingFileMessageBodyWidget::IncomingFileMessageBodyWidget(MessagePtr message)
    : FileMessageBodyWidget(WidgetType::FILE_RECEIVED, message)
{
    button_ = std::make_shared<QPushButton>(DOWNLOAD_FILE_BUTTON_TEXT);
    layout_->addWidget(button_.get());
    buildText(message);
}

std::shared_ptr<QPushButton> IncomingFileMessageBodyWidget::button() const
{
    return button_;
}

std::string IncomingFileMessageBodyWidget::getText(MessagePtr message)
{
    return getStringByFormat("RECEIVE FILE: {}", message->_data());
}

OutgoingFileMessageBodyWidget::OutgoingFileMessageBodyWidget(MessagePtr message)
    : FileMessageBodyWidget(WidgetType::FILE_SENT, message)
{
    buildText(message);
}

std::string OutgoingFileMessageBodyWidget::getText(MessagePtr message)
{
    auto path = getGraphicManager()->getFilePathString(message->networkId());

    return getStringByFormat("SEND FILE: {}", path);
}

MessageWidget::MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : VLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft), companion_(companion), message_(message)
{
    // parent
    if (parent)
        setParent(parent.get());

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

MessagePtr MessageWidget::message()
{
    return message_;
}

TextMessageWidget::TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent) {}

IncomingTextMessageWidget::IncomingTextMessageWidget(
    CompanionPtr companion, IncomingTextMessagePtr message, QWidgetPtr parent)
    : TextMessageWidget(companion, message, parent)
{
    header_ = std::make_shared<IncomingMessageHeaderWidget>(companion, message);
    body_ = std::make_shared<IncomingTextMessageBodyWidget>(message);
    indicatorPanel_ = std::make_shared<IncomingTextMessageIndicatorPanelWidget>(message);

    addWidget(header_.get());
    addWidget(body_.get());
    addWidget(indicatorPanel_.get());
}

void IncomingTextMessageWidget::set(WidgetGroupPtr group)
{
    connect(
        this, &IncomingTextMessageWidget::widgetSelectedSignal,
        group.get(), &WidgetGroup::incomingTextMessageWidgetSelected, Qt::QueuedConnection);
}

void IncomingTextMessageWidget::mousePressEvent(QMouseEvent *event)
{
    indicatorPanel_->unsetNewMessageLabel();
    emit widgetSelectedSignal(shared_from_this());
}

OutgoingTextMessageWidget::OutgoingTextMessageWidget(
    CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : TextMessageWidget(companion, message, parent)
{
    header_ = std::make_shared<OutgoingMessageHeaderWidget>(companion, message);
    body_ = std::make_shared<OutgoingTextMessageBodyWidget>(message);
    indicatorPanel_ = std::make_shared<OutgoingTextMessageIndicatorPanelWidget>(message);

    addWidget(header_.get());
    addWidget(body_.get());
    addWidget(indicatorPanel_.get());
}

void OutgoingTextMessageWidget::set(WidgetGroupPtr group)
{
    connect(
        this, &OutgoingTextMessageWidget::widgetSelectedSignal,
        group.get(), &WidgetGroup::outgoingTextMessageWidgetSelected, Qt::QueuedConnection);
}

void OutgoingTextMessageWidget::mousePressEvent(QMouseEvent *event)
{
    emit widgetSelectedSignal(shared_from_this());
}

void OutgoingTextMessageWidget::setSent()
{
    indicatorPanel_->setSentIndicatorOn();
}

void OutgoingTextMessageWidget::setReceived()
{
    indicatorPanel_->setReceivedIndicatorOn();
}

FileMessageWidget::FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent) {}

IncomingFileMessageWidget::IncomingFileMessageWidget(
    CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : FileMessageWidget(companion, message, parent)
{
    header_ = std::make_shared<IncomingMessageHeaderWidget>(companion, message);
    body_ = std::make_shared<IncomingFileMessageBodyWidget>(message);

    addWidget(header_.get());
    addWidget(body_.get());
}

void IncomingFileMessageWidget::set(WidgetGroupPtr group)
{
    connect(
        body_->button().get(), &QPushButton::clicked,
        this, &IncomingFileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void IncomingFileMessageWidget::saveFileSlot()
{
    // create file operator for this networkId
    auto networkId = message_->networkId();
    getGraphicManager()->saveFile(networkId, companion_);
}

OutgoingFileMessageWidget::OutgoingFileMessageWidget(
    CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : FileMessageWidget(companion, message, parent)
{
    header_ = std::make_shared<OutgoingMessageHeaderWidget>(companion, message);
    body_ = std::make_shared<OutgoingFileMessageBodyWidget>(message);

    addWidget(header_.get());
    addWidget(body_.get());
}
