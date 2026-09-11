#include "widgets_message.hpp"

#include "companion.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "widgets.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool sentByMe, std::shared_ptr<MessageState> state)
{
    sentByMe_ = sentByMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_ = std::make_shared<QHBoxLayout>();
    layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 10, 10);

    setLayout(layout_.get());

    if (sentByMe_) {
        sent_ = std::make_shared<IndicatorWidget>(10, state->isSent());
        received_ = std::make_shared<IndicatorWidget>(10, state->isReceived());

        newMessageLabel_ = nullptr;

        layout_->addWidget(sent_.get());
        layout_->addWidget(received_.get());
    }
    else {
        sent_ = nullptr;
        received_ = nullptr;

        std::string text = (state->isAntecedent()) ? "NEW" : "";

        auto textHtml = getStringByFormat(
            "<font color=\"{0}\"><b>{1}</b></font>", receivedMessageColor, text);

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

MessageWidget::MessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> state, std::shared_ptr<Message> message)
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
    palette_->setColor(QPalette::Window, QColor(messageWidgetBackGroundColor));
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

    indicatorPanel_ = std::make_shared<MessageIndicatorPanelWidget>(isMessageFromMe_, state);
}

void MessageWidget::setBase(std::shared_ptr<WidgetGroup> group)
{
    addMembersToLayout();

    indicatorPanel_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        group.get(), &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

    set(group);
}

void MessageWidget::setMessageWidgetAsSent()
{
    indicatorPanel_->setSentIndicatorOn();
}

void MessageWidget::setMessageWidgetAsReceived()
{
    indicatorPanel_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent *event)
{
    indicatorPanel_->unsetNewMessageLabel();
    emit widgetSelectedSignal(shared_from_this());
}

TextMessageWidget::TextMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> state, std::shared_ptr<Message> message)
    : MessageWidget(parent, companion, state, message)
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

FileMessageWidget::FileMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> state, std::shared_ptr<Message> message)
    : MessageWidget(parent, companion, state, message)
{
    if (parent)
        setParent(parent.get());

    bool isMessageFromMe = message->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    state_ = state;

    // rewrite widget body text for sender's widget
    if (isMessageFromMe) {
        auto pathString = companion->getFileOperatorFilePathStringByNetworkId(
            state->getNetworkId());

        messageLabel_->setText(
            getQString(
                getFormattedMessageBodyString(
                    sentMessageColor,
                    getStringByFormat("SEND FILE: {}", pathString))));
    }

    fileWidget_ = std::make_shared<QWidget>();
    fileWidgetLayout_ = std::make_shared<QHBoxLayout>();
    fileWidget_->setLayout(fileWidgetLayout_.get());

    downloadButton_ = (showButton_) ? std::make_shared<QPushButton>("Download file") : nullptr;
}

void FileMessageWidget::set(std::shared_ptr<WidgetGroup> group)
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
    auto networkId = companion_->getMappedMessageStateByMessage(message_)->getNetworkId();

    getGraphicManager()->saveFile(networkId, companion_);
}
