#include "widgets_message.hpp"

#include "companion.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool sentByMe, MessagePtr message)
{
    sentByMe_ = sentByMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_ = std::make_shared<QHBoxLayout>();
    layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 10, 10);

    setLayout(layout_.get());

    if (sentByMe_) {
        sent_ = std::make_shared<IndicatorWidget>(10, message->isSent());
        received_ = std::make_shared<IndicatorWidget>(10, message->isReceived());

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

}

FileMessageBodyWidget::FileMessageBodyWidget(MessagePtr message)
{

}

MessageWidget::MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
{
    // fields
    type_ = WidgetType::UNKNOWN;
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

    // data
    // auto data = formatMessageHeaderAndBody(companion, message);

    // header_ = std::make_shared<QLabel>(getQString(data.first));
    // header_ = std::make_shared<QLabel>(getQString(data.first));

    // messageLabel_ = std::make_shared<QLabel>(getQString(data.second));

    bool fromMe = message->isMessageFromMe();
    indicatorPanel_ = std::make_shared<MessageIndicatorPanelWidget>(fromMe, message);
}

MessageBodyWidgetPtr MessageWidget::initBody(MessagePtr message)
{
    switch (message->type()) {
    case MessageType::TEXT:
        return std::make_shared<TextMessageBodyWidget>(message);

    case MessageType::FILE:
        return std::make_shared<FileMessageBodyWidget>(message);

    default:
        logArgsError("message widget building error");

        return nullptr;
    }
}

MessagePtr MessageWidget::message()
{
    return message_;
}

void MessageWidget::setBase(WidgetGroupPtr group)
{
    addMembersToLayout();

    indicatorPanel_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        group.get(), &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

    set(group);
}

void MessageWidget::setSent()
{
    indicatorPanel_->setSentIndicatorOn();
}

void MessageWidget::setReceived()
{
    indicatorPanel_->setReceivedIndicatorOn();
}

// std::shared_ptr<QLabel> MessageWidget::messageLabel()
// {
//     return messageLabel_;
// }

void MessageWidget::mousePressEvent(QMouseEvent *event)
{
    indicatorPanel_->unsetNewMessageLabel();
    emit widgetSelectedSignal(shared_from_this());
}

TextMessageWidget::TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent)
{
    if (parent)
        setParent(parent.get());

    type_ = (message->isMessageFromMe()) ? WidgetType::MESSAGE_SENT : WidgetType::MESSAGE_RECEIVED;
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout()
{
    layout_->addWidget(header_.get());
    // layout_->addWidget(messageLabel_.get());
    layout_->addWidget(indicatorPanel_.get());
}

// FileWidget::FileWidget()
// {
//     layout_ = std::make_shared<QHBoxLayout>();
//     setLayout(layout_.get());
// }

// void FileWidget::set()
// {

// }

FileMessageWidget::FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent), /*fileWidget_(new FileWidget),*/
    downloadButton_(nullptr)
{
    if (parent)
        setParent(parent.get());

    // fromMe_ = message->isMessageFromMe();
    type_ = (message->isMessageFromMe()) ? WidgetType::FILE_SENT : WidgetType::FILE_RECEIVED;

    buildText(message);

    fileWidget_ = std::make_shared<QWidget>();
    // fileWidgetLayout_ = std::make_shared<QHBoxLayout>();
    // fileWidget_->setLayout(fileWidgetLayout_.get());

    // button
    if (type_ == WidgetType::FILE_RECEIVED)
        downloadButton_ = std::make_shared<QPushButton>(getButtonText(ButtonType::DOWNLOAD_FILE));
}

void FileMessageWidget::buildText(MessagePtr message)
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
    // messageLabel_->setText(getQString(textHtml));
}

void FileMessageWidget::set(WidgetGroupPtr group)
{
    connect(
        downloadButton_.get(), &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout()
{
    layout_->addWidget(header_.get());
    // fileWidgetLayout_->addWidget(messageLabel_.get());

    // if (type_ == WidgetType::FILE_RECEIVED)
    //     fileWidgetLayout_->addWidget(downloadButton_.get());

    layout_->addWidget(fileWidget_.get());
    layout_->addWidget(indicatorPanel_.get());
}

void FileMessageWidget::saveFileSlot()
{
    // create file operator for this networkId
    auto networkId = message_->networkId();
    getGraphicManager()->saveFile(networkId, companion_);
}

// std::pair<std::string, std::string> formatMessageHeaderAndBody(
//     CompanionPtr companion, MessagePtr message)
// {
//     auto companionName = companion->name();
//     auto companionId = message->companionId();
//     auto authorId = message->authorId();
//     auto time = message->time();
//     auto text = message->text();

//     std::string color, sender, receiver;

//     if (companionId == authorId) {
//         color = getHTMLColor(WidgetType::MESSAGE_RECEIVED);
//         sender = companionName;
//         receiver = "Me";
//     }
//     else {
//         color = getHTMLColor(WidgetType::MESSAGE_SENT);
//         sender = "Me";
//         receiver = companionName;
//     }

//     auto header = getHTMLHeader(color, sender, receiver, time);
//     auto body = getHTMLBody(color, text, true, false);

//     return std::pair<std::string, std::string>(header, body);
// }

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

QString formatBody(CompanionPtr companion, MessagePtr message)
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
