#include "widgets_message.hpp"

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool isMessageFromMe, const MessageState* messageStatePtr) {
    isMessageFromMe_ = isMessageFromMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutPtr_ = new QHBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layoutPtr_->setSpacing(5);
    layoutPtr_->setContentsMargins(0, 0, 10, 10);

    setLayout(layoutPtr_);

    if(isMessageFromMe_) {
        sentIndicatoPtr_ = new IndicatorWidget(10, messageStatePtr->getIsSent());
        receivedIndicatoPtr_ = new IndicatorWidget(10, messageStatePtr->getIsReceived());

        newMessageLabelPtr_ = nullptr;

        layoutPtr_->addWidget(sentIndicatoPtr_);
        layoutPtr_->addWidget(receivedIndicatoPtr_);
    }
    else {
        sentIndicatoPtr_ = nullptr;
        receivedIndicatoPtr_ = nullptr;

        std::string text = (messageStatePtr->getIsAntecedent()) ? "NEW" : "";

        std::string textHtml = std::format(
            "<font color=\"{0}\"><b>{1}</b></font>", receivedMessageColor, text);

        newMessageLabelPtr_ = new QLabel(getQString(textHtml));
        newMessageLabelPtr_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

        layoutPtr_->addWidget(newMessageLabelPtr_);
    }
}

MessageIndicatorPanelWidget::~MessageIndicatorPanelWidget() {
    delete this->layoutPtr_;
    delete this->sentIndicatoPtr_;
    delete this->receivedIndicatoPtr_;
    delete this->newMessageLabelPtr_;
}

void MessageIndicatorPanelWidget::setSentIndicatorOn() {
    this->sentIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn() {
    this->receivedIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel() {
    if(this->newMessageLabelPtr_) {
        this->newMessageLabelPtr_->setText("");
    }
}

MessageWidget::MessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr) {
    companionPtr_ = companionPtr;
    messagePtr_ = messagePtr;

    // createdAsAntecedent_ = messageStatePtr->getIsAntecedent();
    isMessageFromMe_ = messagePtr->isMessageFromMe();

    // set parent
    if(parentPtr) {
        setParent(parentPtr);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColor(messageWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);

    setLayout(layoutPtr_);

    auto data = formatMessageHeaderAndBody(companionPtr, messagePtr);

    headerLabelPtr_ = new QLabel(getQString(data.first));
    messageLabelPtr_ = new QLabel(getQString(data.second));

    indicatorPanelPtr_ = new MessageIndicatorPanelWidget(isMessageFromMe_, messageStatePtr);
}

MessageWidget::~MessageWidget() {
    delete this->palettePtr_;
    delete this->layoutPtr_;
    delete this->headerLabelPtr_;
    delete this->messageLabelPtr_;
    delete this->indicatorPanelPtr_;
}

void MessageWidget::setBase(const WidgetGroup* groupPtr) {
    this->addMembersToLayout();

    this->indicatorPanelPtr_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        groupPtr, &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

    this->set(groupPtr);
}

void MessageWidget::setMessageWidgetAsSent() {
    this->indicatorPanelPtr_->setSentIndicatorOn();
}

void MessageWidget::setMessageWidgetAsReceived() {
    this->indicatorPanelPtr_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent* event) {
    this->indicatorPanelPtr_->unsetNewMessageLabel();
    emit this->widgetSelectedSignal(this);
}

TextMessageWidget::TextMessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr) :
    MessageWidget(parentPtr, companionPtr, messageStatePtr, messagePtr) {
    if(parentPtr) {
        setParent(parentPtr);
    }
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout() {
    layoutPtr_->addWidget(headerLabelPtr_);
    layoutPtr_->addWidget(messageLabelPtr_);
    layoutPtr_->addWidget(indicatorPanelPtr_);
}

FileMessageWidget::FileMessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr) :
    MessageWidget(parentPtr, companionPtr, messageStatePtr, messagePtr) {
    if(parentPtr) {
        setParent(parentPtr);
    }

    bool isMessageFromMe = messagePtr->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    messageStatePtr_ = messageStatePtr;

    // rewrite widget body text for sender's widget
    if(isMessageFromMe) {
        auto pathString = companionPtr->getFileOperatorFilePathStringByNetworkId(
            messageStatePtr->getNetworkId());

        messageLabelPtr_->setText(
            getQString(
                getFormattedMessageBodyString(
                    sentMessageColor,
                    std::format("SEND FILE: {}", pathString))));
    }

    fileWidgetPtr_ = new QWidget;
    fileWidgetLayoutPtr_ = new QHBoxLayout;
    fileWidgetPtr_->setLayout(fileWidgetLayoutPtr_);

    downloadButtonPtr_ = (showButton_) ? new QPushButton("Download file") : nullptr;
}

FileMessageWidget::~FileMessageWidget() {
    delete this->fileWidgetPtr_;
    delete this->fileWidgetLayoutPtr_;
    delete this->downloadButtonPtr_;
}

void FileMessageWidget::set(const WidgetGroup* groupPtr) {
    connect(
        this->downloadButtonPtr_, &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout() {
    layoutPtr_->addWidget(headerLabelPtr_);
    fileWidgetLayoutPtr_->addWidget(messageLabelPtr_);
    logArgs("this->showButton_:", this->showButton_);

    if(this->showButton_) {
        fileWidgetLayoutPtr_->addWidget(downloadButtonPtr_);
    }

    layoutPtr_->addWidget(fileWidgetPtr_);
    layoutPtr_->addWidget(indicatorPanelPtr_);
}

void FileMessageWidget::saveFileSlot() {
    // create file operator for this networkId
    auto networkId = this->companionPtr_->
                     getMappedMessageStatePtrByMessagePtr(this->messagePtr_)->getNetworkId();

    getGraphicManagerPtr()->saveFile(networkId, this->companionPtr_);
}
