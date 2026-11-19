#include "message_widget.hpp"

#include "message.hpp"
#include "widget_group.hpp"

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
