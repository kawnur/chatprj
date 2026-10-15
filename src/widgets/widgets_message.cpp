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

        // auto color = getString(getWidgetColor(Widget::MESSAGE_RECEIVED));
        auto color = getHTMLColor(Widget::MESSAGE_RECEIVED);
        auto text = (message->isAntecedent()) ? "NEW"s : ""s;
        auto textHtml = getHTMLBody(color, text, false, true);
        newMessageLabel_ = std::make_shared<QLabel>(getQString(textHtml));
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

MessageWidget::MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
{
    companion_ = companion;
    message_ = message;

    // createdAsAntecedent_ = messageState->getIsAntecedent();
    isMessageFromMe_ = message->isMessageFromMe();

    // set parent
    if (parent)
        setParent(parent.get());

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(Widget::MESSAGE)));
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_ = std::make_shared<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_.get());

    auto data = formatMessageHeaderAndBody(companion, message);

    headerLabel_ = std::make_shared<QLabel>(getQString(data.first));
    messageLabel_ = std::make_shared<QLabel>(getQString(data.second));

    indicatorPanel_ = std::make_shared<MessageIndicatorPanelWidget>(isMessageFromMe_, message);
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

std::shared_ptr<QLabel> MessageWidget::messageLabel()
{
    return messageLabel_;
}

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
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout()
{
    layout_->addWidget(headerLabel_.get());
    layout_->addWidget(messageLabel_.get());
    layout_->addWidget(indicatorPanel_.get());
}

FileMessageWidget::FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent)
    : MessageWidget(companion, message, parent)
{
    if (parent)
        setParent(parent.get());

    bool isMessageFromMe = message->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    // rewrite widget body text for sender's widget
    if (isMessageFromMe) {
        auto color = getHTMLColor(Widget::MESSAGE_SENT);
        auto path = getGraphicManager()->getFilePathString(message->networkId());
        auto text = getHTMLBody(color, getStringByFormat("SEND FILE: {}", path), true, false);

        messageLabel_->setText(getQString(text));
    }

    fileWidget_ = std::make_shared<QWidget>();
    fileWidgetLayout_ = std::make_shared<QHBoxLayout>();
    fileWidget_->setLayout(fileWidgetLayout_.get());

    downloadButton_ = (showButton_)
        ? std::make_shared<QPushButton>(getButtonText(ButtonType::DOWNLOAD_FILE))
        : nullptr;
}

void FileMessageWidget::set(WidgetGroupPtr group)
{
    connect(
        downloadButton_.get(), &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout()
{
    layout_->addWidget(headerLabel_.get());
    fileWidgetLayout_->addWidget(messageLabel_.get());
    logArgs("showButton_:", showButton_);

    if (showButton_)
        fileWidgetLayout_->addWidget(downloadButton_.get());

    layout_->addWidget(fileWidget_.get());
    layout_->addWidget(indicatorPanel_.get());
}

void FileMessageWidget::saveFileSlot()
{
    // create file operator for this networkId
    auto networkId = message_->networkId();
    getGraphicManager()->saveFile(networkId, companion_);
}

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    CompanionPtr companion, MessagePtr message)
{
    auto companionName = companion->name();
    auto companionId = message->companionId();
    auto authorId = message->authorId();
    auto time = message->time();
    auto text = message->text();

    std::string color, sender, receiver;

    if (companionId == authorId) {
        color = getHTMLColor(Widget::MESSAGE_RECEIVED);
        sender = companionName;
        receiver = "Me";
    }
    else {
        color = getHTMLColor(Widget::MESSAGE_SENT);
        sender = "Me";
        receiver = companionName;
    }

    auto header = getHTMLHeader(color, sender, receiver, time);
    auto body = getHTMLBody(color, text, true, false);

    return std::pair<std::string, std::string>(header, body);
}
