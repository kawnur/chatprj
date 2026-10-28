#include "widgets_message.hpp"

#include "companion.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(MessagePtr message)
{
    sentByMe_ = message->isMessageFromMe();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_ = std::make_shared<QHBoxLayout>();
    layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 10, 10);

    setLayout(layout_.get());

    if (sentByMe_) {
        auto size = MESSAGE_WIDGET_INDICATOR_WIDGET_SIZE;
        sent_ = std::make_shared<IndicatorWidget>(size, message->isSent());
        received_ = std::make_shared<IndicatorWidget>(size, message->isReceived());

        newMessageLabel_ = nullptr;

        layout_->addWidget(sent_.get());
        layout_->addWidget(received_.get());
    }
    else {
        sent_ = nullptr;
        received_ = nullptr;

        auto color = getHTMLColor(WidgetType::MESSAGE_RECEIVED);
        auto text = (message->isAntecedent()) ? "NEW"s : ""s;
        auto textHtml = getHTMLBody(color, text, false, true);
        newMessageLabel_ = std::make_shared<QLabel>(textHtml);
        newMessageLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

        layout_->addWidget(newMessageLabel_.get());
    }
}

void MessageIndicatorPanelWidget::setSentIndicatorOn()
{
    sent_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn()
{
    received_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel()
{
    if (newMessageLabel_)
        newMessageLabel_->setText("");
}

TextMessageBodyWidget::TextMessageBodyWidget(MessagePtr message)
{
    type_ = (message->isMessageFromMe()) ? WidgetType::MESSAGE_SENT : WidgetType::MESSAGE_RECEIVED;
    label_ = std::make_shared<QLabel>(formatBody(message));
}

void TextMessageBodyWidget::addToParentLayout(std::shared_ptr<QVBoxLayout> layout)
{
    layout->addWidget(label_.get());
}

void TextMessageBodyWidget::setParentTo(MessageWidget *widget)
{
    label_->setParent(widget);
}

FileMessageBodyWidget::FileMessageBodyWidget(MessagePtr message)
{
    // type
    type_ = (message->isMessageFromMe()) ? WidgetType::FILE_SENT : WidgetType::FILE_RECEIVED;

    // layout
    layout_ = std::make_shared<QHBoxLayout>();
    setLayout(layout_.get());

    // label
    label_ = std::make_shared<QLabel>();
    layout_->addWidget(label_.get());
    buildText(message);

    // button
    if (type_ == WidgetType::FILE_RECEIVED) {
        button_ = std::make_shared<QPushButton>(getButtonText(ButtonType::DOWNLOAD_FILE));
        layout_->addWidget(button_.get());
    }
}

void FileMessageBodyWidget::addToParentLayout(std::shared_ptr<QVBoxLayout> layout)
{
    layout->addWidget(this);
}

void FileMessageBodyWidget::setParentTo(MessageWidget *widget)
{
    setParent(widget);
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
    auto color = getHTMLColor(type_);
    auto path = getGraphicManager()->getFilePathString(message->networkId());
    auto textTemplate = getFileWidgetText(type_);
    auto text = ""s;

    if (type_ == WidgetType::FILE_SENT)
        text = getStringByFormat(textTemplate, path);
    else if (type_ == WidgetType::FILE_RECEIVED)
        text = getStringByFormat(textTemplate, message->text());

    auto textHtml = getHTMLBody(color, text, true, false);
    label_->setText(textHtml);
}

MessageWidget::MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
{
    // fields
    type_ = getMessageWidgetType(message->type());
    companion_ = companion;
    message_ = message;

    // createdAsAntecedent_ = messageState->getIsAntecedent();

    // parent
    if (parent)
        setParent(parent.get());

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // palette
    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(WidgetType::MESSAGE)));
    setAutoFillBackground(true);
    setPalette(*palette_);

    // layout
    layout_ = std::make_shared<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_.get());

    // header
    header_ = std::make_shared<QLabel>(formatHeader(companion, message));
    layout_->addWidget(header_.get());

    // body
    initBody();
    body_->addToParentLayout(layout_);

    // indicator panel
    indicatorPanel_ = std::make_shared<MessageIndicatorPanelWidget>(message);
    layout_->addWidget(indicatorPanel_.get());
}

void MessageWidget::initBody()
{
    switch (type_) {
    case MessageWidgetType::TEXT:
        body_ = std::make_shared<TextMessageBodyWidget>(message_);

    break;
    case MessageWidgetType::FILE:
        body_ = std::make_shared<FileMessageBodyWidget>(message_);

    break;
    default:
        logArgsError("message widget building error");

    break;
    }
}

MessagePtr MessageWidget::message()
{
    return message_;
}

void MessageWidget::set(WidgetGroupPtr group)
{
    header_->setParent(this);
    body_->setParentTo(this);
    indicatorPanel_->setParent(this);

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

void MessageWidget::setSent()
{
    indicatorPanel_->setSentIndicatorOn();
}

void MessageWidget::setReceived()
{
    indicatorPanel_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent *event)
{
    indicatorPanel_->unsetNewMessageLabel();
    emit widgetSelectedSignal(shared_from_this());
}

QString formatHeader(CompanionPtr companion, MessagePtr message)
{
    auto companionName = companion->name();
    auto time = message->time();

    std::string color, sender, receiver;

    if (message->isMessageFromMe()) {
        color = getHTMLColor(WidgetType::MESSAGE_SENT);
        sender = "Me";
        receiver = companionName;
    } else {
        color = getHTMLColor(WidgetType::MESSAGE_RECEIVED);
        sender = companionName;
        receiver = "Me";
    }

    auto header = getHTMLHeader(color, sender, receiver, time);

    return header;
}

QString formatBody(MessagePtr message)
{
    auto text = message->text();
    std::string color;

    if (message->isMessageFromMe())
        color = getHTMLColor(WidgetType::MESSAGE_SENT);
    else
        color = getHTMLColor(WidgetType::MESSAGE_RECEIVED);

    auto body = getHTMLBody(color, text, true, false);

    return body;
}
