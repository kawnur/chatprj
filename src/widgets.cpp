#include "widgets.hpp"

QString getInitialConnectButtonLabel() {
    try {
        return connectButtonLabels.empty() ?
            "_" : connectButtonLabels.at(0);
    }
    catch(...) {
        return "_?_";
    }
}

QString getNextConnectButtonLabel(QString& currentLabel) {
    try {
        if(connectButtonLabels.empty()) {
            return "";
        }
        else {
            auto currentIterator = std::find(
                connectButtonLabels.begin(),
                connectButtonLabels.end(),
                currentLabel);

            if(currentIterator == connectButtonLabels.end()) {
                return *connectButtonLabels.begin();
            }

            auto nextIterator = currentIterator + 1;

            if(nextIterator == connectButtonLabels.end()) {
                nextIterator = connectButtonLabels.begin();
            }

            return *nextIterator;
        }
    }
    catch(...) {
        return "_?_";
    }
}

TextEditWidget::TextEditWidget() {
    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Base, QColor(textEditBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

TextEditWidget::~TextEditWidget() {
    delete this->palettePtr_;
}

void TextEditWidget::keyPressEvent(QKeyEvent* event) {
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress && event->key() == Qt::Key_Return) {
        if(event->modifiers() == Qt::NoModifier) {
            this->send(this->toPlainText());
            this->setText("");
        }
        else if(event->modifiers() == Qt::ControlModifier) {
            QKeyEvent eventEmulated = QKeyEvent(
                QEvent::KeyPress,
                Qt::Key_Return,
                Qt::NoModifier);

            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

IndicatorWidget::IndicatorWidget(uint8_t size, bool isOn) {
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

IndicatorWidget::IndicatorWidget(const IndicatorWidget* indicator) {
    size_ = indicator->size_;
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palettePtr_ = indicator->palettePtr_;
}

IndicatorWidget::~IndicatorWidget() {
    delete this->palettePtr_;
}

void IndicatorWidget::setOn() {
    this->isOn_ = true;
    this->palettePtr_->setColor(QPalette::Window, onColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setOff() {
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, offColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setMe() {
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, meColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::toggle() {
    (this->isOn_ == true) ? this->setOff() : this->setOn();
}

SocketInfoWidget::SocketInfoWidget() {
    logArgs("SocketInfoWidget()");
}

SocketInfoWidget::SocketInfoWidget(const SocketInfoWidget& si) {
    name_ = si.name_;
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;

    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string& name, std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    name_(getQString(name)), ipAddress_(getQString(ipAddress)),
    serverPort_(serverPort), clientPort_(clientPort) {
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string&& name, std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :
    name_(getQString(name)), ipAddress_(getQString(ipAddress)),
    serverPort_(serverPort), clientPort_(clientPort) {

    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(Companion* companionPtr) :
    companionPtr_(companionPtr),
    name_(getQString(companionPtr->getName())),
    ipAddress_(getQString(companionPtr->getSocketInfoPtr()->getIpAddress())),
    serverPort_(companionPtr->getSocketInfoPtr()->getServerPort()),
    clientPort_(companionPtr->getSocketInfoPtr()->getClientPort()) {

    initializeFields();
}

SocketInfoWidget::~SocketInfoWidget() {
    // cannot set parent
    delete this->palettePtr_;
}

QString SocketInfoWidget::getName() const {
    return this->name_;
}

QString SocketInfoWidget::getIpAddress() const {
    return this->ipAddress_;
}

uint16_t SocketInfoWidget::getServerPort() const {
    return this->serverPort_;
}

uint16_t SocketInfoWidget::getClientPort() const {
    return this->clientPort_;
}

bool SocketInfoWidget::isStub() {
    return false;
}

bool SocketInfoWidget::isSelected() {
    return this->isSelected_;
}

void SocketInfoWidget::select() {
    this->isSelected_ = true;
    this->changeColor(this->selectedColor_);
}

void SocketInfoWidget::unselect() {
    this->isSelected_ = false;
    this->changeColor(this->unselectedColor_);
}

void SocketInfoWidget::update() {
    this->name_ = getQString(this->companionPtr_->getName());
    this->nameLabelPtr_->setText(this->name_);

    this->ipAddress_ = getQString(this->companionPtr_->getSocketIpAddress());

    this->ipAddressLabelPtr_->setText(this->ipAddress_);
    this->clientPort_ = this->companionPtr_->getSocketClientPort();

    this->clientPortLabelPtr_->setText(
        getQString(std::to_string(this->clientPort_)));
}

void SocketInfoWidget::setNewMessagesIndicatorOn() {
    this->newMessagesIndicatorPtr_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff() {
    this->newMessagesIndicatorPtr_->setOff();
}

void SocketInfoWidget::requestHistoryFromCompanionAction() {
    getManagerPtr()->requestHistoryFromCompanion(this->companionPtr_);
}

void SocketInfoWidget::updateCompanionAction() {
    getGraphicManagerPtr()->updateCompanion(this->companionPtr_);
}

void SocketInfoWidget::clearHistoryAction() {
    getGraphicManagerPtr()->clearCompanionHistory(this->companionPtr_);
}

void SocketInfoWidget::deleteCompanionAction() {
    getGraphicManagerPtr()->deleteCompanion(this->companionPtr_);
}

void SocketInfoWidget::clientAction() {
    bool result = false;

    const Companion* companion =
        getManagerPtr()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    QString currentText = this->connectButtonPtr_->text();

    if(this->isConnected_) {
        result = const_cast<Companion*>(companion)->disconnectClient();
    }
    else {
        result = const_cast<Companion*>(companion)->connectClient();
        getManagerPtr()->sendUnsentMessages(companion);
    }

    if(result) {
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

void SocketInfoWidget::initializeFields() {
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

    if(name_ == "me") {  // TODO ???
        connectionStateIndicatorPtr_->setMe();
        editButtonPtr_->hide();
        connectButtonPtr_->hide();
    }

    newMessagesIndicatorPtr_ = new IndicatorWidget(7, false);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    std::initializer_list<QWidget*> widgets {
        connectionStateIndicatorPtr_, nameLabelPtr_, ipAddressLabelPtr_,
        serverPortLabelPtr_, clientPortLabelPtr_, editButtonPtr_,
        connectButtonPtr_, newMessagesIndicatorPtr_
    };

    for(auto& widget : widgets) {
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

void SocketInfoWidget::changeColor(QColor& color) {
    this->palettePtr_ = new QPalette;
    this->palettePtr_->setColor(QPalette::Window, color);

    this->setAutoFillBackground(true);
    this->setPalette(*this->palettePtr_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent* event) {
    Manager* managerPtr = getManagerPtr();

    auto baseObjectPtr = dynamic_cast<SocketInfoBaseWidget*>(this);

    auto newCompanion =
        managerPtr->getMappedCompanionBySocketInfoBaseWidget(baseObjectPtr);

    managerPtr->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent* event) {}

void SocketInfoWidget::customMenuRequestedSlot(QPoint position) {
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

SocketInfoStubWidget::SocketInfoStubWidget() {
    mark_ = getQString(socketInfoStubWidget);

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);

    markLabelPtr_ = new QLabel(mark_);
    layoutPtr_->addWidget(markLabelPtr_);
}

bool SocketInfoStubWidget::isStub() {
    return true;
}

ShowHideWidget::ShowHideWidget() {
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

ShowHideWidget::~ShowHideWidget() {
    delete this->layoutPtr_;
    delete this->labelPtr_;
}

void ShowHideWidget::hideInfo() {
    getGraphicManagerPtr()->hideInfo();
}

void ShowHideWidget::showInfo() {
    getGraphicManagerPtr()->showInfo();
}

void ShowHideWidget::mousePressEvent(QMouseEvent* event) {
    (this->show_) ? this->hideInfo() : this->showInfo();
    this->show_ = !(this->show_);
}

// void ScrollArea::wheelEvent(QWheelEvent* event)
// {
//     logArgs("ScrollArea::wheelEvent");
// }

WidgetGroup::WidgetGroup(Companion* companionPtr) :
    companionPtr_(companionPtr),
    antecedentMessagesCounterMutex_(std::mutex()) {
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

WidgetGroup::~WidgetGroup() {
    getGraphicManagerPtr()->removeWidgetFromCompanionPanel(this->socketInfoBasePtr_);
    delete this->socketInfoBasePtr_;

    // if(this->centralPanelPtr_)  // TODO double destructor call
    // {
    //     delete this->centralPanelPtr_;
    // }
    this->hideCentralPanel();
}

void WidgetGroup::set() {
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
    const Message* messagePtr, const MessageState* messageStatePtr) {
    this->centralPanelPtr_-> addMessageWidgetToChatHistory(
        this, this->companionPtr_, messagePtr, messageStatePtr);
}

void WidgetGroup::clearChatHistory() {
    this->centralPanelPtr_->clearChatHistory();

    dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_)->
        setNewMessagesIndicatorOff();

    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    this->antecedentMessagesCounter_ = 0;
}

void WidgetGroup::hideCentralPanel() {
    this->centralPanelPtr_->hide();
}

void WidgetGroup::showCentralPanel() {
    this->centralPanelPtr_->show();
    this->centralPanelPtr_->scrollDownChatHistory();
}

SocketInfoBaseWidget* WidgetGroup::getSocketInfoBasePtr() {
    return this->socketInfoBasePtr_;
}

void WidgetGroup::sortChatHistoryElements() {
    this->centralPanelPtr_->sortChatHistoryElements(true);
}

void WidgetGroup::messageAdded() {
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    SocketInfoWidget* castPtr =
        dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_);

    if(castPtr && this->antecedentMessagesCounter_ > 0) {
        castPtr->setNewMessagesIndicatorOn();
    }
}

void WidgetGroup::askUserForHistorySendingConfirmation() {
    CompanionAction* actionPtr = new CompanionAction(
        CompanionActionType::SEND_HISTORY,
        getGraphicManagerPtr()->getMainWindowPtr(),
        const_cast<Companion*>(this->companionPtr_));

    actionPtr->set();
}

void WidgetGroup::messageWidgetSelected(MessageWidget* messageWidgetPtr) {
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    MessageState* messageStatePtr =
        const_cast<Companion*>(this->companionPtr_)->
        getMappedMessageStatePtrByMessageWidgetPtr(
            true, messageWidgetPtr);

    bool isAntecedent = messageStatePtr->getIsAntecedent();

    logArgs("isAntecedent:", isAntecedent);

    if(isAntecedent) {
        if(this->antecedentMessagesCounter_ != 0) {
            --this->antecedentMessagesCounter_;
        }

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);

        messageStatePtr->setIsAntecedent(false);

        if(this->antecedentMessagesCounter_ == 0) {
            dynamic_cast<SocketInfoWidget*>(this->socketInfoBasePtr_)->
                setNewMessagesIndicatorOff();
        }
    }
}

void WidgetGroup::buildChatHistorySlot() {
    const_cast<Companion*>(this->companionPtr_)->addMessageWidgetsToChatHistory();
}

void WidgetGroup::addMessageWidgetToCentralPanelChatHistorySlot(
    const MessageState* messageStatePtr, const Message* messagePtr) {
    bool isAntecedent = messageStatePtr->getIsAntecedent();

    if(isAntecedent) {
        std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);
        ++this->antecedentMessagesCounter_;
        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);
    }

    this->centralPanelPtr_->addMessageWidgetToChatHistory(
        this, this->companionPtr_, messagePtr, messageStatePtr);
}

void WidgetGroup::askUserForHistorySendingConfirmationSlot() {
    this->askUserForHistorySendingConfirmation();
}

StubWidgetGroup::StubWidgetGroup() {
    socketInfoPtr_ = new SocketInfoStubWidget;
    leftPanelPtr_ = new LeftPanelWidget(nullptr);
    centralPanelPtr_ = new CentralPanelWidget(nullptr, "");
    rightPanelPtr_ = new RightPanelWidget(nullptr);
}

StubWidgetGroup::~StubWidgetGroup() {
    delete this->socketInfoPtr_;
    delete this->leftPanelPtr_;
    delete this->centralPanelPtr_;
    delete this->rightPanelPtr_;
}

void StubWidgetGroup::set() {
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
    QWidget* leftContainerPtr, QWidget* centralContainerPtr) {
    this->leftPanelPtr_->setParent(centralContainerPtr);
    this->centralPanelPtr_->setParent(centralContainerPtr);
    this->rightPanelPtr_->setParent(centralContainerPtr);
}

void StubWidgetGroup::hideSocketInfoStubWidget() {
    this->socketInfoPtr_->hide();
}

void StubWidgetGroup::hideCentralPanel() {
    this->centralPanelPtr_->hide();
}

void StubWidgetGroup::showCentralPanel() {
    this->centralPanelPtr_->show();
}

void StubWidgetGroup::hideStubPanels() {
    this->leftPanelPtr_->hide();

    if(getManagerPtr()->getSelectedCompanionPtr()) {
        this->centralPanelPtr_->hide();
    }

    this->rightPanelPtr_->hide();
}

void StubWidgetGroup::showStubPanels() {
    this->leftPanelPtr_->show();
    this->centralPanelPtr_->show();
    this->rightPanelPtr_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width) {
    this->leftPanelPtr_->resize(width, this->leftPanelPtr_->height());
}

MainWindowContainerWidget::MainWindowContainerWidget(QWidget* widgetPtr) {
    if(widgetPtr)     {
        setParent(widgetPtr);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);
}

MainWindowContainerWidget::~MainWindowContainerWidget() {
    delete this->layoutPtr_;
}

void MainWindowContainerWidget::addWidgetToLayout(QWidget* widgetPtr) {
    if(widgetPtr) {
        this->layoutPtr_->addWidget(widgetPtr);
    }
}

void MainWindowContainerWidget::addWidgetToLayoutAndSetParentTo(QWidget* widgetPtr) {
    if(widgetPtr) {
        this->layoutPtr_->addWidget(widgetPtr);
        widgetPtr->setParent(this);
    }
}
