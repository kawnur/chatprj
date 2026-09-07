#include "widgets_message.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool isMessageFromMe, std::shared_ptr<MessageState> messageState)
{
    isMessageFromMe_ = isMessageFromMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_ = std::make_shared<QHBoxLayout>();
    layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 10, 10);

    setLayout(layout_.get());

    if (isMessageFromMe_) {
        sentIndicator_ = std::make_shared<IndicatorWidget>(10, messageState->getIsSent());
        receivedIndicator_ = std::make_shared<IndicatorWidget>(10, messageState->getIsReceived());

        newMessageLabel_ = nullptr;

        layout_->addWidget(sentIndicator_);
        layout_->addWidget(receivedIndicator_);
    }
    else {
        sentIndicator_ = nullptr;
        receivedIndicator_ = nullptr;

        std::string text = (messageState->getIsAntecedent()) ? "NEW" : "";

        std::string textHtml = std::format(
            "<font color=\"{0}\"><b>{1}</b></font>", receivedMessageColor, text);

        newMessageLabel_ = std::make_shared<QLabel>(getQString(textHtml));
        newMessageLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

        layout_->addWidget(newMessageLabel_);
    }
}

void MessageIndicatorPanelWidget::setSentIndicatorOn()
{
    sentIndicator_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn()
{
    receivedIndicator_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel()
{
    if (newMessageLabel_)
        newMessageLabel_->setText("");
}

MessageWidget::MessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message)
{
    companion_ = companion;
    message_ = message;

    // createdAsAntecedent_ = messageState->getIsAntecedent();
    isMessageFromMe_ = message->isMessageFromMe();

    // set parent
    if (parent)
        setParent(parent);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, QColor(messageWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_ = std::make_shared<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_);

    auto data = formatMessageHeaderAndBody(companion, message);

    headerLabel_ = std::make_shared<QLabel>(getQString(data.first));
    messageLabel_ = std::make_shared<QLabel>(getQString(data.second));

    indicatorPanel_ = std::make_shared<MessageIndicatorPanelWidget>(isMessageFromMe_, messageState);
}

void MessageWidget::setBase(std::shared_ptr<WidgetGroup> group)
{
    addMembersToLayout();

    indicatorPanel_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        group, &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

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

void MessageWidget::mousePressEvent(QMouseEvent * event)
{
    indicatorPanel_->unsetNewMessageLabel();
    emit widgetSelectedSignal(this);
}

TextMessageWidget::TextMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message)
    : MessageWidget(parent, companion, messageState, message)
{
    if (parent)
        setParent(parent);
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout()
{
    layout_->addWidget(headerLabel_);
    layout_->addWidget(messageLabel_);
    layout_->addWidget(indicatorPanel_);
}

FileMessageWidget::FileMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message)
    : MessageWidget(parent, companion, messageState, message)
{
    if (parent)
        setParent(parent);

    bool isMessageFromMe = message->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    messageState_ = messageState;

    // rewrite widget body text for sender's widget
    if (isMessageFromMe) {
        auto pathString = companion->getFileOperatorFilePathStringByNetworkId(
            messageState->getNetworkId());

        messageLabel_->setText(
            getQString(
                getFormattedMessageBodyString(
                    sentMessageColor,
                    std::format("SEND FILE: {}", pathString))));
    }

    fileWidget_ = std::make_shared<QWidget>();
    fileWidgetLayout_ = std::make_shared<QHBoxLayout>();
    fileWidget_->setLayout(fileWidgetLayout_);

    downloadButton_ = (showButton_) ? std::make_shared<QPushButton>("Download file") : nullptr;
}

void FileMessageWidget::set(std::shared_ptr<WidgetGroup> group)
{
    connect(
        downloadButton_, &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout()
{
    layout_->addWidget(headerLabel_);
    fileWidgetLayout_->addWidget(messageLabel_);
    logArgs("showButton_:", showButton_);

    if (showButton_)
        fileWidgetLayout_->addWidget(downloadButton_);

    layout_->addWidget(fileWidget_);
    layout_->addWidget(indicatorPanel_);
}

void FileMessageWidget::saveFileSlot()
{
    // create file operator for this networkId
    auto networkId = companion_->getMappedMessageStateByMessage(message_)->getNetworkId();

    getGraphicManager()->saveFile(networkId, companion_);
}
