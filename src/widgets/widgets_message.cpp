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

IncomingTextMessageIndicatorPanelWidget::IncomingTextMessageIndicatorPanelWidget(MessagePtr message)
    : MessageIndicatorPanelWidget()
{
    auto color = getHTMLColor(WidgetType::TEXT_RECEIVED);
    auto text = (message->isAntecedent()) ? "NEW"s : ""s;
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

MessageBodyWidget::MessageBodyWidget() : HLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft) {}

TextMessageBodyWidget::TextMessageBodyWidget(MessagePtr message) : MessageBodyWidget()
{
    auto type = (message->isMessageFromMe()) ? WidgetType::TEXT_SENT : WidgetType::TEXT_RECEIVED;
    setType(type);

    label_ = std::make_shared<QLabel>(formatBody(message));
    addWidget(label_.get());
}

FileMessageBodyWidget::FileMessageBodyWidget(MessagePtr message) : MessageBodyWidget()
{
    // type
    auto _type = (message->isMessageFromMe()) ? WidgetType::FILE_SENT : WidgetType::FILE_RECEIVED;
    setType(_type);

    // label
    label_ = std::make_shared<QLabel>();
    addWidget(label_.get());
    buildText(message);

    // button
    if (type() == WidgetType::FILE_RECEIVED) {
        button_ = std::make_shared<QPushButton>(DOWNLOAD_FILE_BUTTON_TEXT);
        layout_->addWidget(button_.get());
    }
}

void FileMessageBodyWidget::set()
{
    connect(
        button_.get(), &QPushButton::clicked,
        this, &FileMessageBodyWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageBodyWidget::saveFileSlot()
{
    auto cast = qobject_cast<MessageWidget *>(parent());

    if (!cast)
        logTemplateError("{}, parent casting error", Q_FUNC_INFO);

    cast->saveFile();
}

void FileMessageBodyWidget::buildText(MessagePtr message)
{
    auto _type = type();

    auto color = getHTMLColor(_type);
    auto path = getGraphicManager()->getFilePathString(message->networkId());
    auto textTemplate = getFileWidgetText(_type);
    auto text = ""s;

    if (_type == WidgetType::FILE_SENT)
        text = getStringByFormat(textTemplate, path);
    else if (_type == WidgetType::FILE_RECEIVED)
        text = getStringByFormat(textTemplate, message->_data());

    auto textHtml = getHTMLBody(color, text, true, false);
    label_->setText(textHtml);
}

MessageWidget::MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : VLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft), companion_(companion), message_(message)
{
    // parent
    if (parent)
        setParent(parent.get());

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // header
    header_ = std::make_shared<QLabel>(formatHeader(companion, message));
}

MessagePtr MessageWidget::message()
{
    return message_;
}

void MessageWidget::set(WidgetGroupPtr group)
{
    connect(
        this, &MessageWidget::widgetSelectedSignal,
        group.get(), &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);
}

void MessageWidget::saveFile()
{
    // create file operator for this networkId
    auto networkId = message_->networkId();
    getGraphicManager()->saveFile(networkId, companion_);
}

void MessageWidget::addMembersToLayout(MessageBodyWidgetPtr body)
{
    body->set();
    addWidget(header_.get());
    addWidget(body.get());
}

TextMessageWidget::TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent)
{
    body_ = std::make_shared<TextMessageBodyWidget>(message_);
    addMembersToLayout(body_);
}

IncomingTextMessageWidget::IncomingTextMessageWidget(
    CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : TextMessageWidget(companion, message, parent)
{
    indicatorPanel_ = std::make_shared<IncomingTextMessageIndicatorPanelWidget>(message);
    addWidget(indicatorPanel_.get());
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
    indicatorPanel_ = std::make_shared<OutgoingTextMessageIndicatorPanelWidget>(message);
    addWidget(indicatorPanel_.get());
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
    : MessageWidget(companion, message, parent)
{
    body_ = std::make_shared<FileMessageBodyWidget>(message_);
    addMembersToLayout(body_);
}

QString formatHeader(CompanionPtr companion, MessagePtr message)
{
    auto companionName = companion->name();
    auto time = message->time();

    std::string color, sender, receiver;

    if (message->isMessageFromMe()) {
        color = getHTMLColor(WidgetType::TEXT_SENT);
        sender = MESSAGE_WIDGET_ME_NAME;
        receiver = companionName;
    } else {
        color = getHTMLColor(WidgetType::TEXT_RECEIVED);
        sender = companionName;
        receiver = MESSAGE_WIDGET_ME_NAME;
    }

    auto header = getHTMLHeader(color, sender, receiver, time);

    return header;
}

QString formatBody(MessagePtr message)
{
    auto text = message->_data();
    std::string color;

    if (message->isMessageFromMe())
        color = getHTMLColor(WidgetType::TEXT_SENT);
    else
        color = getHTMLColor(WidgetType::TEXT_RECEIVED);

    auto body = getHTMLBody(color, text, true, false);

    return body;
}
