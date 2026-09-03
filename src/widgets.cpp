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
    palette_ = new QPalette;
    palette_->setColor(QPalette::Base, QColor(textEditBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);
}

void TextEditWidget::keyPressEvent(std::shared_ptr<QKeyEvent> event) {
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

    palette_ = new QPalette;
    palette_->setColor(QPalette::Window, (isOn_) ? onColor_ : offColor_);

    setAutoFillBackground(true);
    setPalette(*palette_);
}

IndicatorWidget::IndicatorWidget(std::shared_ptr<IndicatorWidget> indicator) {
    size_ = indicator->size_;
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palette_ = indicator->palette_;
}

void IndicatorWidget::setOn() {
    this->isOn_ = true;
    this->palette_->setColor(QPalette::Window, onColor_);
    this->setPalette(*this->palette_);
}

void IndicatorWidget::setOff() {
    this->isOn_ = false;
    this->palette_->setColor(QPalette::Window, offColor_);
    this->setPalette(*this->palette_);
}

void IndicatorWidget::setMe() {
    this->isOn_ = false;
    this->palette_->setColor(QPalette::Window, meColor_);
    this->setPalette(*this->palette_);
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

SocketInfoWidget::SocketInfoWidget(std::shared_ptr<Companion> companion) :
    companion_(companion),
    name_(getQString(companion->getName())),
    ipAddress_(getQString(companion->getSocketInfo()->getIpAddress())),
    serverPort_(companion->getSocketInfo()->getServerPort()),
    clientPort_(companion->getSocketInfo()->getClientPort()) {

    initializeFields();
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
    this->name_ = getQString(this->companion_->getName());
    this->nameLabel_->setText(this->name_);

    this->ipAddress_ = getQString(this->companion_->getSocketIpAddress());

    this->ipAddressLabel_->setText(this->ipAddress_);
    this->clientPort_ = this->companion_->getSocketClientPort();

    this->clientPortLabel_->setText(
        getQString(std::to_string(this->clientPort_)));
}

void SocketInfoWidget::setNewMessagesIndicatorOn() {
    this->newMessagesIndicator_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff() {
    this->newMessagesIndicator_->setOff();
}

void SocketInfoWidget::requestHistoryFromCompanionAction() {
    getManager()->requestHistoryFromCompanion(this->companion_);
}

void SocketInfoWidget::updateCompanionAction() {
    getGraphicManager()->updateCompanion(this->companion_);
}

void SocketInfoWidget::clearHistoryAction() {
    getGraphicManager()->clearCompanionHistory(this->companion_);
}

void SocketInfoWidget::deleteCompanionAction() {
    getGraphicManager()->deleteCompanion(this->companion_);
}

void SocketInfoWidget::clientAction() {
    bool result = false;

    std::shared_ptr<Companion> companion =
        getManager()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    QString currentText = this->connectButton_->text();

    if(this->isConnected_) {
        result = const_cast<std::shared_ptr<Companion>>(companion)->disconnectClient();
    }
    else {
        result = const_cast<std::shared_ptr<Companion>>(companion)->connectClient();
        getManager()->sendUnsentMessages(companion);
    }

    if(result) {
        // change value
        this->isConnected_ = !(this->isConnected_);

        // change connect button text
        QString nextText = getNextConnectButtonLabel(currentText);
        this->connectButton_->setText(nextText);

        // change indicator color
        this->connectionStateIndicator_->toggle();

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
    palette_ = new QPalette;
    palette_->setColor(QPalette::Window, unselectedColor_);
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_ = new QHBoxLayout;
    setLayout(layout_);
    connectionStateIndicator_ = new IndicatorWidget(15, false);
    nameLabel_ = new QLabel(name_);
    ipAddressLabel_ = new QLabel(ipAddress_);

    QString serverPortQString = getQString(std::to_string(serverPort_));
    QString clientPortQString = getQString(std::to_string(clientPort_));

    serverPortLabel_ = new QLabel(serverPortQString);
    clientPortLabel_ = new QLabel(clientPortQString);
    editButton_ = new QPushButton("Edit");
    connectButton_ = new QPushButton(getInitialConnectButtonLabel());

    connect(
        this->editButton_, &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        this->connectButton_, &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction, Qt::QueuedConnection);

    if(name_ == "me") {  // TODO ???
        connectionStateIndicator_->setMe();
        editButton_->hide();
        connectButton_->hide();
    }

    newMessagesIndicator_ = new IndicatorWidget(7, false);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    std::initializer_list<std::shared_ptr<QWidget>> widgets {
        connectionStateIndicator_, nameLabel_, ipAddressLabel_,
        serverPortLabel_, clientPortLabel_, editButton_,
        connectButton_, newMessagesIndicator_
    };

    for(auto& widget : widgets) {
        layout_->addWidget(widget);
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
    this->palette_ = new QPalette;
    this->palette_->setColor(QPalette::Window, color);

    this->setAutoFillBackground(true);
    this->setPalette(*this->palette_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent * event) {
    std::shared_ptr<Manager> manager = getManager();

    auto baseObject = dynamic_cast<std::shared_ptr<SocketInfoBaseWidget>>(this);

    auto newCompanion =
        manager->getMappedCompanionBySocketInfoBaseWidget(baseObject);

    manager->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(std::shared_ptr<QMouseEvent> event) {}

void SocketInfoWidget::customMenuRequestedSlot(QPoint position) {
    std::shared_ptr<QMenu> menu = new QMenu(this);

    menu->addAction(this->requestHistoryAction_);

    std::shared_ptr<QAction> clearHistoryAction = new QAction("Clear chat history", this);
    menu->addAction(clearHistoryAction);

    connect(
        clearHistoryAction, &QAction::triggered,
        this, &SocketInfoWidget::clearHistoryAction, Qt::QueuedConnection);

    std::shared_ptr<QAction> deleteCompanionAction = new QAction("Delete companion", this);
    menu->addAction(deleteCompanionAction);

    connect(
        deleteCompanionAction, &QAction::triggered,
        this, &SocketInfoWidget::deleteCompanionAction, Qt::QueuedConnection);

    menu->popup(this->mapToGlobal(position));
}

SocketInfoStubWidget::SocketInfoStubWidget() {
    mark_ = getQString(socketInfoStubWidget);

    layout_ = new QHBoxLayout;
    setLayout(layout_);

    markLabel_ = new QLabel(mark_);
    layout_->addWidget(markLabel_);
}

bool SocketInfoStubWidget::isStub() {
    return true;
}

ShowHideWidget::ShowHideWidget() {
    show_ = true;
    layout_ = new QVBoxLayout;
    layout_->setAlignment(Qt::AlignCenter);
    setLayout(layout_);

    label_ = new QLabel;
    label_->setText("Show / Hide");

    palette_ = new QPalette;
    palette_->setColor(QPalette::Window, QColor(showHideWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_->addWidget(label_);
}

void ShowHideWidget::hideInfo() {
    getGraphicManager()->hideInfo();
}

void ShowHideWidget::showInfo() {
    getGraphicManager()->showInfo();
}

void ShowHideWidget::mousePressEvent(QMouseEvent * event) {
    (this->show_) ? this->hideInfo() : this->showInfo();
    this->show_ = !(this->show_);
}

// void ScrollArea::wheelEvent(std::shared_ptr<QWheelEvent> event)
// {
//     logArgs("ScrollArea::wheelEvent");
// }

WidgetGroup::WidgetGroup(std::shared_ptr<Companion> companion) :
    companion_(companion),
    antecedentMessagesCounterMutex_(std::mutex()) {
    antecedentMessagesCounter_ = 0;

    std::shared_ptr<GraphicManager> graphicManager = getGraphicManager();

    std::shared_ptr<SocketInfo> socketInfo = companion_->getSocketInfo();

    std::shared_ptr<SocketInfoWidget> widget = new SocketInfoWidget(
        const_cast<std::shared_ptr<Companion>>(companion_));

    socketInfoBase_ = dynamic_cast<std::shared_ptr<SocketInfoBaseWidget>>(widget);
    graphicManager->addWidgetToCompanionPanel(socketInfoBase_);

    centralPanel_ = new CentralPanelWidget(
        getGraphicManager()->getMainWindow(), companion_->getName());

    // centralPanel_ = new CentralPanelWidget(nullptr, companion_->getName());

    centralPanel_->set(const_cast<std::shared_ptr<Companion>>(companion_));

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, centralPanel_);

    centralPanel_->hide();
}

WidgetGroup::~WidgetGroup()
{
    getGraphicManager()->removeWidgetFromCompanionPanel(this->socketInfoBase_);
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
    std::shared_ptr<Message> message, std::shared_ptr<MessageState> messageState) {
    this->centralPanel_->addMessageWidgetToChatHistory(
        this, this->companion_, message, messageState);
}

void WidgetGroup::clearChatHistory() {
    this->centralPanel_->clearChatHistory();

    dynamic_cast<std::shared_ptr<SocketInfoWidget>>(this->socketInfoBase_)->
        setNewMessagesIndicatorOff();

    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    this->antecedentMessagesCounter_ = 0;
}

void WidgetGroup::hideCentralPanel() {
    this->centralPanel_->hide();
}

void WidgetGroup::showCentralPanel() {
    this->centralPanel_->show();
    this->centralPanel_->scrollDownChatHistory();
}

std::shared_ptr<SocketInfoBaseWidget> WidgetGroup::getSocketInfoBase() {
    return this->socketInfoBase_;
}

void WidgetGroup::sortChatHistoryElements() {
    this->centralPanel_->sortChatHistoryElements(true);
}

void WidgetGroup::messageAdded() {
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    std::shared_ptr<SocketInfoWidget> cast =
        dynamic_cast<std::shared_ptr<SocketInfoWidget>>(this->socketInfoBase_);

    if(cast && this->antecedentMessagesCounter_ > 0) {
        cast->setNewMessagesIndicatorOn();
    }
}

void WidgetGroup::askUserForHistorySendingConfirmation() {
    std::shared_ptr<CompanionAction> action = new CompanionAction(
        ChatActionType::SEND_HISTORY,
        const_cast<std::shared_ptr<Companion>>(this->companion_));

    action->set();
}

void WidgetGroup::messageWidgetSelected(std::shared_ptr<MessageWidget> messageWidget) {
    std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);

    std::shared_ptr<MessageState> messageState =
        const_cast<std::shared_ptr<Companion>>(this->companion_)->
        getMappedMessageStateByMessageWidget(
            true, messageWidget);

    bool isAntecedent = messageState->getIsAntecedent();

    logArgs("isAntecedent:", isAntecedent);

    if(isAntecedent) {
        if(this->antecedentMessagesCounter_ != 0) {
            --this->antecedentMessagesCounter_;
        }

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);

        messageState->setIsAntecedent(false);

        if(this->antecedentMessagesCounter_ == 0) {
            dynamic_cast<std::shared_ptr<SocketInfoWidget>>(this->socketInfoBase_)->
                setNewMessagesIndicatorOff();
        }
    }
}

void WidgetGroup::buildChatHistorySlot() {
    const_cast<std::shared_ptr<Companion>>(this->companion_)->addMessageWidgetsToChatHistory();
}

void WidgetGroup::addMessageWidgetToCentralPanelChatHistorySlot(
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message) {
    bool isAntecedent = messageState->getIsAntecedent();

    if(isAntecedent) {
        std::lock_guard<std::mutex> lock(this->antecedentMessagesCounterMutex_);
        ++this->antecedentMessagesCounter_;
        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);
    }

    this->centralPanel_->addMessageWidgetToChatHistory(
        this, this->companion_, message, messageState);
}

void WidgetGroup::askUserForHistorySendingConfirmationSlot() {
    this->askUserForHistorySendingConfirmation();
}

StubWidgetGroup::StubWidgetGroup()
{
    socketInfo_ = new SocketInfoStubWidget;
    leftPanel_ = new LeftPanelWidget(nullptr);
    centralPanel_ = new CentralPanelWidget(nullptr, "");
    rightPanel_ = new RightPanelWidget(nullptr);
}

void StubWidgetGroup::set() {
    std::shared_ptr<GraphicManager> graphicManager = getGraphicManager();

    std::shared_ptr<SocketInfoBaseWidget> baseObjectCast =
        dynamic_cast<std::shared_ptr<SocketInfoBaseWidget>>(socketInfo_);

    graphicManager->addWidgetToCompanionPanel(baseObjectCast);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, this->leftPanel_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, this->centralPanel_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::RIGHT, this->rightPanel_);

    leftPanel_->hide();
    centralPanel_->hide();
    rightPanel_->hide();
}

void StubWidgetGroup::setParents(
    std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer) {
    this->leftPanel_->setParent(centralContainer);
    this->centralPanel_->setParent(centralContainer);
    this->rightPanel_->setParent(centralContainer);
}

void StubWidgetGroup::hideSocketInfoStubWidget() {
    this->socketInfo_->hide();
}

void StubWidgetGroup::hideCentralPanel() {
    this->centralPanel_->hide();
}

void StubWidgetGroup::showCentralPanel() {
    this->centralPanel_->show();
}

void StubWidgetGroup::hideStubPanels() {
    this->leftPanel_->hide();

    if(getManager()->getSelectedCompanion()) {
        this->centralPanel_->hide();
    }

    this->rightPanel_->hide();
}

void StubWidgetGroup::showStubPanels() {
    this->leftPanel_->show();
    this->centralPanel_->show();
    this->rightPanel_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width) {
    this->leftPanel_->resize(width, this->leftPanel_->height());
}

MainWindowContainerWidget::MainWindowContainerWidget(std::shared_ptr<QWidget> widget) {
    if(widget)     {
        setParent(widget);
    }

    layout_ = new QVBoxLayout;
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

void MainWindowContainerWidget::addWidgetToLayout(std::shared_ptr<QWidget> widget) {
    if(widget) {
        this->layout_->addWidget(widget);
    }
}

void MainWindowContainerWidget::addWidgetToLayoutAndSetParentTo(std::shared_ptr<QWidget> widget) {
    if(widget) {
        this->layout_->addWidget(widget);
        widget->setParent(this);
    }
}
