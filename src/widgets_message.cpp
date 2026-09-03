#include "widgets_message.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool isMessageFromMe, std::shared_ptr<MessageState> messageState) {
    isMessageFromMe_ = isMessageFromMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_ = new QHBoxLayout;
    layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout_->setSpacing(5);
    layout_->setContentsMargins(0, 0, 10, 10);

    setLayout(layout_);

    if(isMessageFromMe_) {
        sentIndicato_ = new IndicatorWidget(10, messageState->getIsSent());
        receivedIndicato_ = new IndicatorWidget(10, messageState->getIsReceived());

        newMessageLabel_ = nullptr;

        layout_->addWidget(sentIndicato_);
        layout_->addWidget(receivedIndicato_);
    }
    else {
        sentIndicato_ = nullptr;
        receivedIndicato_ = nullptr;

        std::string text = (messageState->getIsAntecedent()) ? "NEW" : "";

        std::string textHtml = std::format(
            "<font color=\"{0}\"><b>{1}</b></font>", receivedMessageColor, text);

        newMessageLabel_ = new QLabel(getQString(textHtml));
        newMessageLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

        layout_->addWidget(newMessageLabel_);
    }
}

void MessageIndicatorPanelWidget::setSentIndicatorOn() {
    this->sentIndicato_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn() {
    this->receivedIndicato_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel() {
    if(this->newMessageLabel_) {
        this->newMessageLabel_->setText("");
    }
}

MessageWidget::MessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message) {
    companion_ = companion;
    message_ = message;

    // createdAsAntecedent_ = messageState->getIsAntecedent();
    isMessageFromMe_ = message->isMessageFromMe();

    // set parent
    if(parent) {
        setParent(parent);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    palette_ = new QPalette;
    palette_->setColor(QPalette::Window, QColor(messageWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_ = new QVBoxLayout;
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_);

    auto data = formatMessageHeaderAndBody(companion, message);

    headerLabel_ = new QLabel(getQString(data.first));
    messageLabel_ = new QLabel(getQString(data.second));

    indicatorPanel_ = new MessageIndicatorPanelWidget(isMessageFromMe_, messageState);
}

void MessageWidget::setBase(std::shared_ptr<WidgetGroup> group) {
    this->addMembersToLayout();

    this->indicatorPanel_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        group, &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

    this->set(group);
}

void MessageWidget::setMessageWidgetAsSent() {
    this->indicatorPanel_->setSentIndicatorOn();
}

void MessageWidget::setMessageWidgetAsReceived() {
    this->indicatorPanel_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent * event) {
    this->indicatorPanel_->unsetNewMessageLabel();
    emit this->widgetSelectedSignal(this);
}

TextMessageWidget::TextMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message) :
    MessageWidget(parent, companion, messageState, message) {
    if(parent) {
        setParent(parent);
    }
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout() {
    layout_->addWidget(headerLabel_);
    layout_->addWidget(messageLabel_);
    layout_->addWidget(indicatorPanel_);
}

FileMessageWidget::FileMessageWidget(
    std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message) :
    MessageWidget(parent, companion, messageState, message) {
    if(parent) {
        setParent(parent);
    }

    bool isMessageFromMe = message->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    messageState_ = messageState;

    // rewrite widget body text for sender's widget
    if(isMessageFromMe) {
        auto pathString = companion->getFileOperatorFilePathStringByNetworkId(
            messageState->getNetworkId());

        messageLabel_->setText(
            getQString(
                getFormattedMessageBodyString(
                    sentMessageColor,
                    std::format("SEND FILE: {}", pathString))));
    }

    fileWidget_ = new QWidget;
    fileWidgetLayout_ = new QHBoxLayout;
    fileWidget_->setLayout(fileWidgetLayout_);

    downloadButton_ = (showButton_) ? new QPushButton("Download file") : nullptr;
}

void FileMessageWidget::set(std::shared_ptr<WidgetGroup> group) {
    connect(
        this->downloadButton_, &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout() {
    layout_->addWidget(headerLabel_);
    fileWidgetLayout_->addWidget(messageLabel_);
    logArgs("this->showButton_:", this->showButton_);

    if(this->showButton_) {
        fileWidgetLayout_->addWidget(downloadButton_);
    }

    layout_->addWidget(fileWidget_);
    layout_->addWidget(indicatorPanel_);
}

void FileMessageWidget::saveFileSlot() {
    // create file operator for this networkId
    auto networkId = this->companion_->
                     getMappedMessageStateByMessage(this->message_)->getNetworkId();

    getGraphicManager()->saveFile(networkId, this->companion_);
}
