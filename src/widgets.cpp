#include "widgets.hpp"

QString getInitialConnectButtonLabel()
{
    try
    {
        return connectButtonLabels.empty() ?
            "_" : connectButtonLabels.at(0);
    }
    catch(...)
    {
        return "_?_";
    }
}

QString getNextConnectButtonLabel(QString& currentLabel)
{
    try
    {
        if(connectButtonLabels.empty())
        {
            return "";
        }
        else
        {
            auto currentIterator = std::find(
                connectButtonLabels.begin(),
                connectButtonLabels.end(),
                currentLabel);

            if(currentIterator == connectButtonLabels.end())
            {
                return *connectButtonLabels.begin();
            }

            auto nextIterator = currentIterator + 1;

            if(nextIterator == connectButtonLabels.end())
            {
                nextIterator = connectButtonLabels.begin();
            }

            return *nextIterator;
        }
    }
    catch(...)
    {
        return "_?_";
    }
}

TextEditWidget::TextEditWidget()
{
    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Base, QColor(textEditBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

TextEditWidget::~TextEditWidget()
{
    delete this->palettePtr_;
}

void TextEditWidget::keyPressEvent(QKeyEvent* event)
{
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress && event->key() == Qt::Key_Return)
    {
        if(event->modifiers() == Qt::NoModifier)
        {
            this->send(this->toPlainText());
            this->setText("");
        }
        else if(event->modifiers() == Qt::ControlModifier)
        {
            QKeyEvent eventEmulated = QKeyEvent(
                QEvent::KeyPress,
                Qt::Key_Return,
                Qt::NoModifier);

            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}

IndicatorWidget::IndicatorWidget(uint8_t size, bool isOn)
{
    size_ = size;
    setFixedWidth(size_);
    setFixedHeight(size_);

    isOn_ = isOn;
    onColor_ = QColor(QColorConstants::DarkGreen);
    offColor_ = QColor(QColorConstants::DarkRed);
    meColor_ = QColor(indicatorMeColor);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, (isOn_) ? onColor_ : offColor_);

    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

IndicatorWidget::IndicatorWidget(const IndicatorWidget* indicator)
{
    size_ = indicator->size_;
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palettePtr_ = indicator->palettePtr_;
}

IndicatorWidget::~IndicatorWidget()
{
    delete this->palettePtr_;
}

void IndicatorWidget::setOn()
{
    this->isOn_ = true;
    this->palettePtr_->setColor(QPalette::Window, onColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setOff()
{
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, offColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setMe()
{
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, meColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::toggle()
{
    (this->isOn_ == true) ? this->setOff() : this->setOn();
}

SocketInfoWidget::SocketInfoWidget()
{
    logArgs("SocketInfoWidget()");
}

SocketInfoWidget::SocketInfoWidget(const SocketInfoWidget& si)
{
    name_ = si.name_;
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;

    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string& name, std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    name_(getQString(name)), ipAddress_(getQString(ipAddress)),
    serverPort_(serverPort), clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string&& name, std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :
    name_(getQString(name)), ipAddress_(getQString(ipAddress)),
    serverPort_(serverPort), clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(Companion* companionPtr) :
    companionPtr_(companionPtr),
    name_(getQString(companionPtr->getName())),
    ipAddress_(getQString(companionPtr->getSocketInfoPtr()->getIpAddress())),
    serverPort_(companionPtr->getSocketInfoPtr()->getServerPort()),
    clientPort_(companionPtr->getSocketInfoPtr()->getClientPort())
{
    initializeFields();
}

SocketInfoWidget::~SocketInfoWidget()
{
    // cannot set parent
    delete this->palettePtr_;
}

QString SocketInfoWidget::getName() const
{
    return this->name_;
}

QString SocketInfoWidget::getIpAddress() const
{
    return this->ipAddress_;
}

uint16_t SocketInfoWidget::getServerPort() const
{
    return this->serverPort_;
}

uint16_t SocketInfoWidget::getClientPort() const
{
    return this->clientPort_;
}

bool SocketInfoWidget::isStub()
{
    return false;
}

bool SocketInfoWidget::isSelected()
{
    return this->isSelected_;
}

void SocketInfoWidget::select()
{
    this->isSelected_ = true;
    this->changeColor(this->selectedColor_);
}

void SocketInfoWidget::unselect()
{
    this->isSelected_ = false;
    this->changeColor(this->unselectedColor_);
}

void SocketInfoWidget::update()
{
    this->name_ = getQString(this->companionPtr_->getName());
    this->nameLabelPtr_->setText(this->name_);

    this->ipAddress_ = getQString(this->companionPtr_->getSocketIpAddress());

    this->ipAddressLabelPtr_->setText(this->ipAddress_);
    this->clientPort_ = this->companionPtr_->getSocketClientPort();

    this->clientPortLabelPtr_->setText(
        getQString(std::to_string(this->clientPort_)));
}

void SocketInfoWidget::setNewMessagesIndicatorOn()
{
    this->newMessagesIndicatorPtr_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff()
{
    this->newMessagesIndicatorPtr_->setOff();
}

void SocketInfoWidget::requestHistoryFromCompanionAction()
{
    getManagerPtr()->requestHistoryFromCompanion(this->companionPtr_);
}

void SocketInfoWidget::updateCompanionAction()
{
    getGraphicManagerPtr()->updateCompanion(this->companionPtr_);
}

void SocketInfoWidget::clearHistoryAction()
{
    getGraphicManagerPtr()->clearCompanionHistory(this->companionPtr_);
}

void SocketInfoWidget::deleteCompanionAction()
{
    getGraphicManagerPtr()->deleteCompanion(this->companionPtr_);
}

void SocketInfoWidget::clientAction()
{
    bool result = false;

    const Companion* companion =
        getManagerPtr()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    QString currentText = this->connectButtonPtr_->text();

    if(this->isConnected_)
    {
        result = const_cast<Companion*>(companion)->disconnectClient();
    }
    else
    {
        result = const_cast<Companion*>(companion)->connectClient();
        getManagerPtr()->sendUnsentMessages(companion);
    }

    if(result)
    {
        // change value
        this->isConnected_ = !(this->isConnected_);

        // change connect button text
        QString nextText = getNextConnectButtonLabel(currentText);
        this->connectButtonPtr_->setText(nextText);

        // change indicator color
        this->connectionStateIndicatorPtr_->toggle();

        // set context menu action enabled
        this->requestHistoryAction_->setDisabled(
            this->requestHistoryAction_->isEnabled());
    }
}

void SocketInfoWidget::initializeFields()
{
    isSelected_ = false;
    isConnected_ = false;

    selectedColor_ = QColor(QColorConstants::DarkGray);
    unselectedColor_ = QColor(QColorConstants::Gray);
    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, unselectedColor_);
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);
    connectionStateIndicatorPtr_ = new IndicatorWidget(15, false);
    nameLabelPtr_ = new QLabel(name_);
    ipAddressLabelPtr_ = new QLabel(ipAddress_);

    QString serverPortQString = getQString(std::to_string(serverPort_));
    QString clientPortQString = getQString(std::to_string(clientPort_));

    serverPortLabelPtr_ = new QLabel(serverPortQString);
    clientPortLabelPtr_ = new QLabel(clientPortQString);
    editButtonPtr_ = new QPushButton("Edit");
    connectButtonPtr_ = new QPushButton(getInitialConnectButtonLabel());

    connect(
        this->editButtonPtr_, &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        this->connectButtonPtr_, &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction, Qt::QueuedConnection);

    if(name_ == "me")  // TODO ???
    {
        connectionStateIndicatorPtr_->setMe();
        editButtonPtr_->hide();
        connectButtonPtr_->hide();
    }

    newMessagesIndicatorPtr_ = new IndicatorWidget(7, false);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    std::initializer_list<QWidget*> widgets
    {
        connectionStateIndicatorPtr_, nameLabelPtr_, ipAddressLabelPtr_,
        serverPortLabelPtr_, clientPortLabelPtr_, editButtonPtr_,
        connectButtonPtr_, newMessagesIndicatorPtr_
    };

    for(auto& widget : widgets)
    {
        layoutPtr_->addWidget(widget);
    }

    requestHistoryAction_ = new QAction("Request chat history from companion", this);
    requestHistoryAction_->setDisabled(true);

    connect(
        this->requestHistoryAction_, &QAction::triggered,
        this, &SocketInfoWidget::requestHistoryFromCompanionAction,
        Qt::QueuedConnection);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, &QWidget::customContextMenuRequested,
        this, &SocketInfoWidget::customMenuRequestedSlot, Qt::QueuedConnection);

    // connect
}

void SocketInfoWidget::changeColor(QColor& color)
{
    this->palettePtr_ = new QPalette;
    this->palettePtr_->setColor(QPalette::Window, color);

    this->setAutoFillBackground(true);
    this->setPalette(*this->palettePtr_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent* event)
{
    Manager* managerPtr = getManagerPtr();

    auto baseObjectPtr = dynamic_cast<SocketInfoBaseWidget*>(this);

    auto newCompanion =
        managerPtr->getMappedCompanionBySocketInfoBaseWidget(baseObjectPtr);

    managerPtr->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent* event) {}

void SocketInfoWidget::customMenuRequestedSlot(QPoint position)
{
    QMenu* menu = new QMenu(this);

    menu->addAction(this->requestHistoryAction_);

    QAction* clearHistoryAction = new QAction("Clear chat history", this);
    menu->addAction(clearHistoryAction);

    connect(
        clearHistoryAction, &QAction::triggered,
        this, &SocketInfoWidget::clearHistoryAction, Qt::QueuedConnection);

    QAction* deleteCompanionAction = new QAction("Delete companion", this);
    menu->addAction(deleteCompanionAction);

    connect(
        deleteCompanionAction, &QAction::triggered,
        this, &SocketInfoWidget::deleteCompanionAction, Qt::QueuedConnection);

    menu->popup(this->mapToGlobal(position));
}

SocketInfoStubWidget::SocketInfoStubWidget()
{
    mark_ = getQString(socketInfoStubWidget);

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);

    markLabelPtr_ = new QLabel(mark_);
    layoutPtr_->addWidget(markLabelPtr_);
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

ShowHideWidget::ShowHideWidget()
{
    show_ = true;
    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignCenter);
    setLayout(layoutPtr_);

    labelPtr_ = new QLabel;
    labelPtr_->setText("Show / Hide");

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColor(showHideWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_->addWidget(labelPtr_);
}

ShowHideWidget::~ShowHideWidget()
{
    delete this->layoutPtr_;
    delete this->labelPtr_;
}

void ShowHideWidget::hideInfo()
{
    getGraphicManagerPtr()->hideInfo();
}

void ShowHideWidget::showInfo()
{
    getGraphicManagerPtr()->showInfo();
}

void ShowHideWidget::mousePressEvent(QMouseEvent* event)
{
    (this->show_) ? this->hideInfo() : this->showInfo();
    this->show_ = !(this->show_);
}

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool isMessageFromMe, const MessageState* messageStatePtr)
{
    isMessageFromMe_ = isMessageFromMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutPtr_ = new QHBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layoutPtr_->setSpacing(5);
    layoutPtr_->setContentsMargins(0, 0, 10, 10);

    setLayout(layoutPtr_);

    if(isMessageFromMe_)
    {
        sentIndicatoPtr_ = new IndicatorWidget(10, messageStatePtr->getIsSent());
        receivedIndicatoPtr_ = new IndicatorWidget(10, messageStatePtr->getIsReceived());

        newMessageLabelPtr_ = nullptr;

        layoutPtr_->addWidget(sentIndicatoPtr_);
        layoutPtr_->addWidget(receivedIndicatoPtr_);
    }
    else
    {
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

MessageIndicatorPanelWidget::~MessageIndicatorPanelWidget()
{
    delete this->layoutPtr_;
    delete this->sentIndicatoPtr_;
    delete this->receivedIndicatoPtr_;
    delete this->newMessageLabelPtr_;
}

void MessageIndicatorPanelWidget::setSentIndicatorOn()
{
    this->sentIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn()
{
    this->receivedIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel()
{
    if(this->newMessageLabelPtr_)
    {
        this->newMessageLabelPtr_->setText("");
    }
}

MessageWidget::MessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr)
{
    companionPtr_ = companionPtr;
    messagePtr_ = messagePtr;

    // createdAsAntecedent_ = messageStatePtr->getIsAntecedent();
    isMessageFromMe_ = messagePtr->isMessageFromMe();

    // set parent
    if(parentPtr)
    {
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

MessageWidget::~MessageWidget()
{
    delete this->palettePtr_;
    delete this->layoutPtr_;
    delete this->headerLabelPtr_;
    delete this->messageLabelPtr_;
    delete this->indicatorPanelPtr_;
}

void MessageWidget::setBase(const WidgetGroup* groupPtr)
{
    this->addMembersToLayout();

    this->indicatorPanelPtr_->setParent(this);

    connect(
        this, &MessageWidget::widgetSelectedSignal,
        groupPtr, &WidgetGroup::messageWidgetSelected, Qt::QueuedConnection);

    this->set(groupPtr);
}

void MessageWidget::setMessageWidgetAsSent()
{
    this->indicatorPanelPtr_->setSentIndicatorOn();
}

void MessageWidget::setMessageWidgetAsReceived()
{
    this->indicatorPanelPtr_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent* event)
{
    this->indicatorPanelPtr_->unsetNewMessageLabel();

    emit this->widgetSelectedSignal(this);
}

TextMessageWidget::TextMessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr) :
    MessageWidget(parentPtr, companionPtr, messageStatePtr, messagePtr)
{
    if(parentPtr)
    {
        setParent(parentPtr);
    }
}

TextMessageWidget::~TextMessageWidget() {}

void TextMessageWidget::addMembersToLayout()
{
    layoutPtr_->addWidget(headerLabelPtr_);
    layoutPtr_->addWidget(messageLabelPtr_);
    layoutPtr_->addWidget(indicatorPanelPtr_);
}

FileMessageWidget::FileMessageWidget(
    QWidget* parentPtr, Companion* companionPtr,
    const MessageState* messageStatePtr, const Message* messagePtr) :
    MessageWidget(parentPtr, companionPtr, messageStatePtr, messagePtr)
{
    if(parentPtr)
    {
        setParent(parentPtr);
    }

    bool isMessageFromMe = messagePtr->isMessageFromMe();

    showButton_ = !isMessageFromMe;

    messageStatePtr_ = messageStatePtr;

    // rewrite widget body text for sender's widget
    if(isMessageFromMe)
    {
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

FileMessageWidget::~FileMessageWidget()
{
    delete this->fileWidgetPtr_;
    delete this->fileWidgetLayoutPtr_;
    delete this->downloadButtonPtr_;
}

void FileMessageWidget::set(const WidgetGroup* groupPtr)
{
    connect(
        this->downloadButtonPtr_, &QPushButton::clicked,
        this, &FileMessageWidget::saveFileSlot, Qt::QueuedConnection);
}

void FileMessageWidget::addMembersToLayout()
{
    layoutPtr_->addWidget(headerLabelPtr_);

    fileWidgetLayoutPtr_->addWidget(messageLabelPtr_);

    logArgs("this->showButton_:", this->showButton_);

    if(this->showButton_)
    {
        fileWidgetLayoutPtr_->addWidget(downloadButtonPtr_);
    }

    layoutPtr_->addWidget(fileWidgetPtr_);

    layoutPtr_->addWidget(indicatorPanelPtr_);
}

void FileMessageWidget::saveFileSlot()
{
    // create file operator for this networkId
    auto networkId = this->companionPtr_->
        getMappedMessageStatePtrByMessagePtr(this->messagePtr_)->getNetworkId();

    getGraphicManagerPtr()->saveFile(networkId, this->companionPtr_);
}

LeftPanelWidget::LeftPanelWidget(QWidget* parent)
{
    if(parent)
    {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);

    setLayout(layoutPtr_);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColor(leftPanelBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    companionPanelPtr_ = new QWidget;
    // leftPanelPtr_->setStyleSheet("border-right: 1px solid black");
    companionPanelPtr_->resize(2000, 1000);  // TODO ???
    companionPanelLayoutPtr_ = new QVBoxLayout;
    companionPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    companionPanelLayoutPtr_->setSpacing(0);
    companionPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    companionPanelPtr_->setLayout(companionPanelLayoutPtr_);

    layoutPtr_->addWidget(companionPanelPtr_);

    companionPanelPtr_->resize(4000, 1000);  // TODO ???

    spacerPtr_ = new QSpacerItem(
        0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    layoutPtr_->addSpacerItem(spacerPtr_);
}

LeftPanelWidget::~LeftPanelWidget()
{
    delete this->layoutPtr_;
    delete this->palettePtr_;
    delete this->companionPanelPtr_;
    delete this->companionPanelLayoutPtr_;
    delete this->spacerPtr_;
}

void LeftPanelWidget::addWidgetToCompanionPanel(SocketInfoBaseWidget* widget)
{
    this->companionPanelLayoutPtr_->addWidget(widget);
}

size_t LeftPanelWidget::getCompanionPanelChildrenSize()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void LeftPanelWidget::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widgetPtr);

    if(index == -1)
    {
        showErrorDialogAndLogError(
            nullptr, "SocketInfoBaseWidget was not found in companion panel");
    }
    else if(index == 0)
    {

    }
    else if(index > 0)
    {
        Manager* managerPtr = getManagerPtr();

        auto previousWidget = companionPanelChildren.at(index - 1);

        auto previousCompanionPtr =
            managerPtr->getMappedCompanionBySocketInfoBaseWidget(previousWidget);

        managerPtr->resetSelectedCompanion(previousCompanionPtr);

        this->companionPanelLayoutPtr_->removeWidget(widgetPtr);
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    if(companionPanelChildren.size() == 0)
    {
        return -1;
    }
    else
    {
        return companionPanelChildren.at(companionPanelChildren.size() - 1)->width();
    }
}

// void ScrollArea::wheelEvent(QWheelEvent* event)
// {
//     logArgs("ScrollArea::wheelEvent");
// }

CentralPanelWidget::CentralPanelWidget(QWidget* parent, const std::string& name) :
    chatHistoryMutex_(std::mutex())
{
    chatHistoryScrollAreaPtr_ = nullptr;
    chatHistoryWidgetPalettePtr_ = nullptr;

    if(parent)
    {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    companionNameLabelPtr_ = new QLabel(getQString(name));
    // companionNameLabelPtr_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalettePtr_ = new QPalette;
    companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
    companionNameLabelPtr_->setAutoFillBackground(true);
    companionNameLabelPtr_->setPalette(*companionNameLabelPalettePtr_);

    layoutPtr_->addWidget(companionNameLabelPtr_);

    chatHistoryWidgetPtr_ = new QWidget;

    chatHistoryLayoutPtr_ = new QVBoxLayout;
    chatHistoryLayoutPtr_->setSpacing(0);
    chatHistoryLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    chatHistoryLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMaximumSize);
    chatHistoryWidgetPtr_->setLayout(chatHistoryLayoutPtr_);

    if(name.size() != 0)
    {
        chatHistoryScrollAreaPtr_ = new QScrollArea;
        // chatHistoryScrollAreaPtr_ = new ScrollArea;
        chatHistoryScrollAreaPtr_->setWidgetResizable(true);
        chatHistoryScrollAreaPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        chatHistoryScrollAreaPtr_->setWidget(chatHistoryWidgetPtr_);
        chatHistoryWidgetPalettePtr_ = new QPalette;
        chatHistoryWidgetPalettePtr_->setColor(QPalette::Window, QColorConstants::Gray);
        chatHistoryWidgetPtr_->setPalette(*chatHistoryWidgetPalettePtr_);

        layoutPtr_->addWidget(chatHistoryScrollAreaPtr_);
    }

    buttonPanelWidgetPtr_ = new QWidget;
    buttonPanelLayoutPtr_ = new QHBoxLayout;
    buttonPanelLayoutPtr_->setSpacing(10);
    buttonPanelLayoutPtr_->setContentsMargins(10, 10, 10, 10);
    buttonPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buttonPanelWidgetPtr_->setLayout(buttonPanelLayoutPtr_);
    buttonPanelPalettePtr_ = new QPalette;
    buttonPanelPalettePtr_->setColor(QPalette::Window, QColor(buttonPanelBackGroundColor));
    chatHistoryWidgetPtr_->setPalette(*buttonPanelPalettePtr_);
    sendFileButtonPtr_ = new QPushButton("Send file");
    buttonPanelLayoutPtr_->addWidget(sendFileButtonPtr_);

    layoutPtr_->addWidget(buttonPanelWidgetPtr_);

    textEditPtr_ = new TextEditWidget;
    layoutPtr_->addWidget(textEditPtr_);

    // splitterPtr_ = new QSplitter;
    // layoutPtr_->addWidget(splitterPtr_);
    // splitterPtr_->addWidget(borderWidgetPtr_);
    // splitterPtr_->addWidget(textEditPtr_);
}

CentralPanelWidget::~CentralPanelWidget()
{
    delete this->layoutPtr_;
    delete this->companionNameLabelPtr_;
    delete this->companionNameLabelPalettePtr_;
    delete this->chatHistoryWidgetPtr_;
    delete this->chatHistoryScrollAreaPtr_;
    delete this->chatHistoryWidgetPalettePtr_;
    delete this->chatHistoryLayoutPtr_;
    delete this->textEditPtr_;
    delete this->textEditPalettePtr_;
    delete this->buttonPanelWidgetPtr_;
    delete this->buttonPanelLayoutPtr_;
    delete this->buttonPanelPalettePtr_;
    delete this->sendFileButtonPtr_;

    // TODO delete message widgets
}

void CentralPanelWidget::set(Companion* companionPtr)
{
    this->companionPtr_ = companionPtr;

    connect(
        this->textEditPtr_, &TextEditWidget::send,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        this->sendFileButtonPtr_, &QPushButton::clicked,
        this, &CentralPanelWidget::sendFileSlot, Qt::QueuedConnection);

    this->chatHistoryScrollAreaPtr_->installEventFilter(this);
}

void CentralPanelWidget::addMessageWidgetToChatHistory(
    const WidgetGroup* widgetGroupPtr, Companion* companionPtr,
    const Message* messagePtr, const MessageState* messageStatePtr)
{
    {
        std::lock_guard<std::mutex> lock(this->chatHistoryMutex_);

        MessageWidget* widgetPtr = nullptr;

        switch(messagePtr->getType())
        {
        case MessageType::TEXT:
            widgetPtr = new TextMessageWidget(
                this->chatHistoryWidgetPtr_, companionPtr, messageStatePtr, messagePtr);

            break;

        case MessageType::FILE:
            widgetPtr = new FileMessageWidget(
                this->chatHistoryWidgetPtr_, companionPtr, messageStatePtr, messagePtr);

            break;
        }

        std::thread(
            [=]()
            {
                companionPtr->setMappedMessageWidget(messagePtr, widgetPtr);
            }
        ).detach();

        if(widgetGroupPtr)
        {
            widgetPtr->setBase(widgetGroupPtr);
        }

        this->chatHistoryLayoutPtr_->addWidget(widgetPtr);

        if(messageStatePtr->getIsAntecedent())
        {
            this->sortChatHistoryElements(false);
        }
    }

    // widget group action
    if(widgetGroupPtr)
    {
        const_cast<WidgetGroup*>(widgetGroupPtr)->messageAdded();
    }

    this->scrollDownChatHistory();
}

void CentralPanelWidget::scrollDownChatHistory()
{
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    this->chatHistoryScrollAreaPtr_->verticalScrollBar()->setValue(
        this->chatHistoryScrollAreaPtr_->verticalScrollBar()->maximum());
}

void CentralPanelWidget::clearChatHistory()
{
    auto children = this->chatHistoryWidgetPtr_->children();

    for(auto& child : children)
    {
        MessageWidget* messageWidgetPtr = dynamic_cast<MessageWidget*>(child);

        if(messageWidgetPtr)
        {
            messageWidgetPtr->hide();
            delete messageWidgetPtr;
        }
    }
}

void CentralPanelWidget::sortChatHistoryElements(bool lock)
{
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->chatHistoryMutex_);

    auto list = this->chatHistoryWidgetPtr_->children();

    auto lambda = [&](auto item)
    {
        const Message* messagePtr =
            companionPtr_->getMappedMessagePtrByMessageWidgetPtr(
                false, dynamic_cast<MessageWidget*>(item));

        return (messagePtr) ? messagePtr->getTime() : std::string("");
    };

    std::sort(
        list.begin(),
        list.end(),
        [&](auto element1, auto element2)
        {
            auto res1 = lambda(element1);
            auto res2 = lambda(element2);
            bool res = (res1 < res2);

            coutArgsWithSpaceSeparator("res1:", res1, "res2:", res2, "res:", res);

            return res;
        });

    coutArgsWithSpaceSeparator("AFTER SORTING");

    for(auto& element : list)
    {
        auto elementCast = dynamic_cast<MessageWidget*>(element);

        const Message* messagePtr =
            companionPtr_->getMappedMessagePtrByMessageWidgetPtr(false, elementCast);

        coutArgsWithSpaceSeparator("messagePtr:", messagePtr);

        if(messagePtr)
        {
            coutArgsWithSpaceSeparator("element message:", messagePtr->getText());
        }

        this->chatHistoryLayoutPtr_->removeWidget(elementCast);
        this->chatHistoryLayoutPtr_->addWidget(elementCast);
    }
}

bool CentralPanelWidget::eventFilter(QObject* objectPtr, QEvent* eventPtr)
{
    auto result = QWidget::eventFilter(objectPtr, eventPtr);

    if(objectPtr == this->chatHistoryScrollAreaPtr_)
    {
        auto verticalScrollBarPtr =
            this->chatHistoryScrollAreaPtr_->verticalScrollBar();

        if(verticalScrollBarPtr &&
            verticalScrollBarPtr->value() == verticalScrollBarPtr->minimum())
        {
            QKeyEvent* eventCastPtr = dynamic_cast<QKeyEvent*>(eventPtr);

            if(eventPtr->type() == QEvent::Wheel ||
                (eventPtr->type() == QEvent::KeyPress &&
                eventCastPtr &&
                (eventCastPtr->key() &
                (Qt::Key_Up | Qt::Key_PageUp | Qt::Key_Home))))
            {
                logArgsWithTemplate(
                    "scroll bar minimum event type {}", std::to_string(eventPtr->type()));

                getManagerPtr()->addEarlyMessages(this->companionPtr_);
            }
        }
    }

    return result;
}

void CentralPanelWidget::sendMessage(const QString& text)
{
    if(!text.isEmpty())
    {
        getGraphicManagerPtr()->sendMessage(
            MessageType::TEXT, this->companionPtr_, text.toStdString());
    }
}

void CentralPanelWidget::sendFileSlot()
{
    getGraphicManagerPtr()->sendFile(this->companionPtr_);
}

void CentralPanelWidget::saveFileSlot()
{
    getGraphicManagerPtr()->sendFile(this->companionPtr_);
}

RightPanelWidget::RightPanelWidget(QWidget* parent)
{
    if(parent)
    {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    appLogWidgetPtr_ = new QPlainTextEdit;
    // appLogWidgetPtr_->setStyleSheet("border-left: 1px solid black");
    appLogWidgetPtr_->setReadOnly(true);
    appLogWidgetPtr_->setPlainText("");

    appLogWidgetPalettePtr_ = new QPalette;
    appLogWidgetPalettePtr_->setColor(QPalette::Base, QColor(appLogBackgroundColor));
    appLogWidgetPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidgetPtr_->setAutoFillBackground(true);
    appLogWidgetPtr_->setPalette(*appLogWidgetPalettePtr_);

    layoutPtr_->addWidget(appLogWidgetPtr_);

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);
}

RightPanelWidget::~RightPanelWidget()
{
    delete this->layoutPtr_;
    delete this->appLogWidgetPtr_;
    delete this->appLogWidgetPalettePtr_;
}

void RightPanelWidget::set()
{
    this->appLogWidgetPtr_->setParent(this);

    connect(
        this, SIGNAL(addTextToAppLogWidgetSignal(const QString&)),
        this, SLOT(addTextToAppLogWidgetSlot(const QString&)),
        Qt::QueuedConnection);

    this->appLogWidgetPtr_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this->appLogWidgetPtr_, &QWidget::customContextMenuRequested,
        this, &RightPanelWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void RightPanelWidget::addTextToAppLogWidget(const QString& text)
{
    emit this->addTextToAppLogWidgetSignal(text);
}

void RightPanelWidget::clearLogAction()
{
    this->appLogWidgetPtr_->clear();
}

void RightPanelWidget::addTextToAppLogWidgetSlot(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);

    // QApplication::processEvents();

    this->appLogWidgetPtr_->ensureCursorVisible();
}

void RightPanelWidget::customMenuRequestedSlot(QPoint position)
{
    QMenu* menu = new QMenu(this);

    QAction* clearLogAction = new QAction("Clear log", this);
    menu->addAction(clearLogAction);

    connect(
        clearLogAction, &QAction::triggered,
        this, &RightPanelWidget::clearLogAction, Qt::QueuedConnection);

    menu->popup(this->mapToGlobal(position));
}

WidgetGroup::WidgetGroup(Companion* companionPtr) :
    companionPtr_(companionPtr),
    antecedentMessagesCounterMutex_(std::mutex())
{
    antecedentMessagesCounter_ = 0;

    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    const SocketInfo* socketInfoPtr = companionPtr_->getSocketInfoPtr();

    SocketInfoWidget* widget = new SocketInfoWidget(
        const_cast<Companion*>(companionPtr_));

    socketInfoBasePtr_ = dynamic_cast<SocketInfoBaseWidget*>(widget);
    graphicManagerPtr->addWidgetToCompanionPanel(socketInfoBasePtr_);

    centralPanelPtr_ = new CentralPanelWidget(
        getGraphicManagerPtr()->getMainWindowPtr(), companionPtr_->getName());

    // centralPanelPtr_ = new CentralPanelWidget(nullptr, companionPtr_->getName());

    centralPanelPtr_->set(const_cast<Companion*>(companionPtr_));

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, centralPanelPtr_);

    centralPanelPtr_->hide();
}

WidgetGroup::~WidgetGroup()
{
    getGraphicManagerPtr()->removeWidgetFromCompanionPanel(this->socketInfoBasePtr_);
    delete this->socketInfoBasePtr_;

    // if(this->centralPanelPtr_)  // TODO double destructor call
    // {
    //     delete this->centralPanelPtr_;
    // }
    this->hideCentralPanel();
}

void WidgetGroup::set()
{
    connect(
        this, &WidgetGroup::addMessageWidgetToCentralPanelChatHistorySignal,
        this, &WidgetGroup::addMessageWidgetToCentralPanelChatHistorySlot,
        Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::askUserForHistorySendingConfirmationSignal,
        this, &WidgetGroup::askUserForHistorySendingConfirmationSlot,
        Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::buildChatHistorySignal,
        this, &WidgetGroup::buildChatHistorySlot,
        Qt::QueuedConnection);
}

void WidgetGroup::addMessageWidgetToCentralPanelChatHistory(
    const Message* messagePtr, const MessageState* messageStatePtr)
{
    this->centralPanelPtr_-> addMessageWidgetToChatHistory(
        this, this->companionPtr_, messagePtr, messageStatePtr);
}

void WidgetGroup::clearChatHistory()
{
    this->centralPanelPtr_->clearChatHistory();

    dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_)->
        setNewMessagesIndicatorOff();

    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    this->antecedentMessagesCounter_ = 0;
}

void WidgetGroup::hideCentralPanel()
{
    this->centralPanelPtr_->hide();
}

void WidgetGroup::showCentralPanel()
{
    this->centralPanelPtr_->show();
    this->centralPanelPtr_->scrollDownChatHistory();
}

SocketInfoBaseWidget* WidgetGroup::getSocketInfoBasePtr()
{
    return this->socketInfoBasePtr_;
}

void WidgetGroup::sortChatHistoryElements()
{
    this->centralPanelPtr_->sortChatHistoryElements(true);
}

void WidgetGroup::messageAdded()
{
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    SocketInfoWidget* castPtr =
        dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_);

    if(castPtr && this->antecedentMessagesCounter_ > 0)
    {
        castPtr->setNewMessagesIndicatorOn();
    }
}

void WidgetGroup::askUserForHistorySendingConfirmation()
{
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::SEND_HISTORY,
        getGraphicManagerPtr()->getMainWindowPtr(),
        const_cast<Companion*>(this->companionPtr_));

    actionPtr->set();
}

void WidgetGroup::messageWidgetSelected(MessageWidget* messageWidgetPtr)
{
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    MessageState* messageStatePtr =
        const_cast<Companion*>(this->companionPtr_)->
        getMappedMessageStatePtrByMessageWidgetPtr(
            true, messageWidgetPtr);

    bool isAntecedent = messageStatePtr->getIsAntecedent();

    logArgs("isAntecedent:", isAntecedent);

    if(isAntecedent)
    {
        if(this->antecedentMessagesCounter_ != 0)
        {
            --this->antecedentMessagesCounter_;
        }

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);

        messageStatePtr->setIsAntecedent(false);

        if(this->antecedentMessagesCounter_ == 0)
        {
            dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_)->
                setNewMessagesIndicatorOff();
        }
    }
}

void WidgetGroup::buildChatHistorySlot()
{
    const_cast<Companion*>(this->companionPtr_)->addMessageWidgetsToChatHistory();
}

void WidgetGroup::addMessageWidgetToCentralPanelChatHistorySlot(
    const MessageState* messageStatePtr, const Message* messagePtr)
{
    bool isAntecedent = messageStatePtr->getIsAntecedent();

    if(isAntecedent)
    {
        std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);
        ++this->antecedentMessagesCounter_;
        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);
    }

    this->centralPanelPtr_->addMessageWidgetToChatHistory(
        this, this->companionPtr_, messagePtr, messageStatePtr);
}

void WidgetGroup::askUserForHistorySendingConfirmationSlot()
{
    this->askUserForHistorySendingConfirmation();
}

StubWidgetGroup::StubWidgetGroup()
{
    socketInfoPtr_ = new SocketInfoStubWidget;
    leftPanelPtr_ = new LeftPanelWidget(nullptr);
    centralPanelPtr_ = new CentralPanelWidget(nullptr, "");
    rightPanelPtr_ = new RightPanelWidget(nullptr);
}

StubWidgetGroup::~StubWidgetGroup()
{
    delete this->socketInfoPtr_;
    delete this->leftPanelPtr_;
    delete this->centralPanelPtr_;
    delete this->rightPanelPtr_;
}

void StubWidgetGroup::set()
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    SocketInfoBaseWidget* baseObjectCastPtr =
        dynamic_cast<SocketInfoBaseWidget*>(socketInfoPtr_);

    graphicManagerPtr->addWidgetToCompanionPanel(baseObjectCastPtr);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, this->leftPanelPtr_);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, this->centralPanelPtr_);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::RIGHT, this->rightPanelPtr_);

    leftPanelPtr_->hide();
    centralPanelPtr_->hide();
    rightPanelPtr_->hide();
}

void StubWidgetGroup::setParents(
    QWidget* leftContainerPtr, QWidget* centralContainerPtr)
{
    this->leftPanelPtr_->setParent(centralContainerPtr);
    this->centralPanelPtr_->setParent(centralContainerPtr);
    this->rightPanelPtr_->setParent(centralContainerPtr);
}

void StubWidgetGroup::hideSocketInfoStubWidget()
{
    this->socketInfoPtr_->hide();
}

void StubWidgetGroup::hideCentralPanel()
{
    this->centralPanelPtr_->hide();
}

void StubWidgetGroup::showCentralPanel()
{
    this->centralPanelPtr_->show();
}

void StubWidgetGroup::hideStubPanels()
{
    this->leftPanelPtr_->hide();

    if(getManagerPtr()->getSelectedCompanionPtr())
    {
        this->centralPanelPtr_->hide();
    }

    this->rightPanelPtr_->hide();
}

void StubWidgetGroup::showStubPanels()
{
    this->leftPanelPtr_->show();
    this->centralPanelPtr_->show();
    this->rightPanelPtr_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width)
{
    this->leftPanelPtr_->resize(width, this->leftPanelPtr_->height());
}

MainWindowContainerWidget::MainWindowContainerWidget(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        setParent(widgetPtr);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);
}

MainWindowContainerWidget::~MainWindowContainerWidget()
{
    delete this->layoutPtr_;
}

void MainWindowContainerWidget::addWidgetToLayout(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        this->layoutPtr_->addWidget(widgetPtr);
    }
}

void MainWindowContainerWidget::addWidgetToLayoutAndSetParentTo(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        this->layoutPtr_->addWidget(widgetPtr);
        widgetPtr->setParent(this);
    }
}

CompanionDataDialog::CompanionDataDialog(
    CompanionActionType actionType, QWidget* parentPtr, Companion* companionPtr)
{
    setParent(parentPtr);

    setWindowTitle(
        getConstantMappingValue(
            "actionTypeStringRepresentation",
            &actionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    nameLabelPtr_ = new QLabel("Name");
    nameEditPtr_ = new QLineEdit;

    ipAddressLabelPtr_ = new QLabel("IpAddress");
    ipAddressEditPtr_ = new QLineEdit;

    portLabelPtr_ = new QLabel("Port");
    portEditPtr_ = new QLineEdit;

    if(actionType_ == CompanionActionType::UPDATE && companionPtr)
    {
        nameEditPtr_->setText(getQString(companionPtr->getName()));

        ipAddressEditPtr_->setText(
            getQString(companionPtr->getSocketInfoPtr()->getIpAddress()));

        portEditPtr_->setText(
            getQString(std::to_string(
                companionPtr->getSocketInfoPtr()->getClientPort())));
    }

    layoutPtr_->addRow(nameLabelPtr_, nameEditPtr_);
    layoutPtr_->addRow(ipAddressLabelPtr_, ipAddressEditPtr_);
    layoutPtr_->addRow(portLabelPtr_, portEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CompanionDataDialog::~CompanionDataDialog()
{
    delete this->layoutPtr_;
    delete this->nameLabelPtr_;
    delete this->nameEditPtr_;
    delete this->ipAddressLabelPtr_;
    delete this->ipAddressEditPtr_;
    delete this->portLabelPtr_;
    delete this->portEditPtr_;
    delete this->buttonBoxPtr_;
}

void CompanionDataDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);

    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::rejected,
        this, &QDialog::reject, Qt::QueuedConnection);
}

std::string CompanionDataDialog::getNameString()
{
    return this->nameEditPtr_->text().toStdString();
}

std::string CompanionDataDialog::getIpAddressString()
{
    auto ipAddressFromWidget = this->ipAddressEditPtr_->text().toStdString();  // TODO change
    QHostAddress hostAddress { getQString(ipAddressFromWidget) };

    return hostAddress.toString().toStdString();
}

std::string CompanionDataDialog::getPortString()
{
    return this->portEditPtr_->text().toStdString();
}

CreatePasswordDialog::CreatePasswordDialog()
{
    setWindowTitle(newPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    firstLabelPtr_ = new QLabel(newPasswordDialogFirstLabel);
    firstEditPtr_ = new QLineEdit;

    secondLabelPtr_ = new QLabel(newPasswordDialogSecondLabel);
    secondEditPtr_ = new QLineEdit;

    layoutPtr_->addRow(firstLabelPtr_, firstEditPtr_);
    layoutPtr_->addRow(secondLabelPtr_, secondEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CreatePasswordDialog::~CreatePasswordDialog()
{
    delete this->layoutPtr_;
    delete this->firstLabelPtr_;
    delete this->firstEditPtr_;
    delete this->secondLabelPtr_;
    delete this->secondEditPtr_;
    delete this->buttonBoxPtr_;
}

void CreatePasswordDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::getFirstEditText()
{
    return this->firstEditPtr_->text().toStdString();
}

std::string CreatePasswordDialog::getSecondEditText()
{
    return this->secondEditPtr_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog()
{
    setWindowTitle(getPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    labelPtr_ = new QLabel(getPasswordDialogLabel);
    editPtr_ = new QLineEdit;

    layoutPtr_->addRow(labelPtr_, editPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

GetPasswordDialog::~GetPasswordDialog()
{
    delete this->layoutPtr_;
    delete this->labelPtr_;
    delete this->editPtr_;
    delete this->buttonBoxPtr_;
}

void GetPasswordDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string GetPasswordDialog::getEditText()
{
    return this->editPtr_->text().toStdString();
}

ButtonInfo::ButtonInfo(
    const QString& buttonText, QDialogButtonBox::ButtonRole buttonRole,
    void (TextDialog::*functionPtr)()) :
    buttonText_(buttonText), buttonRole_(buttonRole), functionPtr_(functionPtr) {}

TextDialog::TextDialog(
    QWidget* parentPtr, DialogType dialogType, const std::string& text,
    std::vector<ButtonInfo>* buttonsInfoPtr)
{
    if(parentPtr)
    {
        setParent(parentPtr);
    }
    else
    {
        setParent(getGraphicManagerPtr()->getMainWindowPtr());
    }

    setModal(true);
    setWindowFlag(Qt::Window);

    setWindowTitle(
        getConstantMappingValue(
            "dialogTypeStringRepresentation",
            &dialogTypeStringRepresentation,
            dialogType));

    layoutPtr_ = new QVBoxLayout;
    setLayout(layoutPtr_);

    textEditPtr_ = new QPlainTextEdit;
    textEditPtr_->setReadOnly(true);
    textEditPtr_->setPlainText(getQString(text));
    layoutPtr_->addWidget(textEditPtr_);

    // set button box
    buttonBoxPtr_ = new QDialogButtonBox;
    layoutPtr_->addWidget(buttonBoxPtr_);

    buttonsInfoPtr_ = buttonsInfoPtr;
}

TextDialog::~TextDialog()
{
    delete this->layoutPtr_;
    delete this->textEditPtr_;
    delete this->buttonBoxPtr_;
    delete this->buttonsInfoPtr_;
}

void TextDialog::set()
{
    for(auto& info : *this->buttonsInfoPtr_)
    {
        QPushButton* buttonPtr = this->buttonBoxPtr_->addButton(
            info.buttonText_, info.buttonRole_);

        if(info.buttonRole_ == QDialogButtonBox::AcceptRole)
        {
            connect(
                this->buttonBoxPtr_, &QDialogButtonBox::accepted,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else if(info.buttonRole_ == QDialogButtonBox::RejectRole)
        {
            connect(
                this->buttonBoxPtr_, &QDialogButtonBox::rejected,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else
        {
            showErrorDialogAndLogError(nullptr, "Unmanaged button role");
        }
    }
}

void TextDialog::closeSelf()
{
    this->close();
}

void TextDialog::closeSelfAndParentDialog()
{
    this->close();

    auto parentPtr = this->parent();

    if(parentPtr)
    {
        dynamic_cast<QWidget*>(parentPtr)->close();
    }
}

void TextDialog::acceptAction()
{
    this->close();
    this->actionPtr_->sendData();
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs()
{
    getGraphicManagerPtr()->disableMainWindowBlurEffect();
    this->closeSelfAndParentDialog();
}

void TextDialog::reject()
{
    QDialog::reject();
}

FileDialog::FileDialog(FileAction* actionPtr, const QString& windowTitle)
{
    actionPtr_ = actionPtr;

    containsDialogPtr_ = true;
    fileDialogPtr_ = new QFileDialog;
    fileDialogPtr_->setFileMode(QFileDialog::AnyFile);
    fileDialogPtr_->setViewMode(QFileDialog::Detail);

    fileDialogPtr_->
        setDirectory(getQString(getManagerPtr()->getLastOpenedPath().string()));

    fileDialogPtr_->setWindowTitle(windowTitle);
}

FileDialog::~FileDialog()
{
    delete this->fileDialogPtr_;
}

void FileDialog::set()
{
    connect(
        this->fileDialogPtr_, &QFileDialog::accepted,
        this->actionPtr_, &Action::sendData,
        Qt::QueuedConnection);
}

void FileDialog::showDialog()
{
    this->fileDialogPtr_->show();
}

QFileDialog* FileDialog::getFileDialogPtr()
{
    return this->fileDialogPtr_;
}
