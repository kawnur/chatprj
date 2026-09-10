#include "widgets.hpp"

#include <QMenu>

#include "action.hpp"
#include "companion.hpp"
#include "constants.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets_panel.hpp"

QString getInitialConnectButtonLabel()
{
    try {
        return connectButtonLabels.empty() ? "_" : connectButtonLabels.at(0);
    }
    catch(...) {
        return "_?_";
    }
}

QString getNextConnectButtonLabel(QString& currentLabel)
{
    try {
        if (connectButtonLabels.empty()) {
            return "";
        }
        else {
            auto currentIterator = std::find(
                connectButtonLabels.begin(),
                connectButtonLabels.end(),
                currentLabel);

            if (currentIterator == connectButtonLabels.end())
                return *connectButtonLabels.begin();

            auto nextIterator = currentIterator + 1;

            if (nextIterator == connectButtonLabels.end())
                nextIterator = connectButtonLabels.begin();

            return *nextIterator;
        }
    }
    catch(...) {
        return "_?_";
    }
}

TextEditWidget::TextEditWidget()
{
    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Base, QColor(textEditBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);
}

void TextEditWidget::keyPressEvent(QKeyEvent *event)
{
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if (event->type() == QEvent::KeyPress && event->key() == Qt::Key_Return) {
        if (event->modifiers() == Qt::NoModifier) {
            send(toPlainText());
            setText("");
        }
        else if (event->modifiers() == Qt::ControlModifier) {
            QKeyEvent eventEmulated = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else {
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

    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, (isOn_) ? onColor_ : offColor_);

    setAutoFillBackground(true);
    setPalette(*palette_);
}

IndicatorWidget::IndicatorWidget(std::shared_ptr<IndicatorWidget> indicator)
{
    size_ = indicator->size_;
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palette_ = indicator->palette_;
}

void IndicatorWidget::setOn()
{
    isOn_ = true;
    palette_->setColor(QPalette::Window, onColor_);
    setPalette(*palette_);
}

void IndicatorWidget::setOff()
{
    isOn_ = false;
    palette_->setColor(QPalette::Window, offColor_);
    setPalette(*palette_);
}

void IndicatorWidget::setMe()
{
    isOn_ = false;
    palette_->setColor(QPalette::Window, meColor_);
    setPalette(*palette_);
}

void IndicatorWidget::toggle()
{
    (isOn_ == true) ? setOff() : setOn();
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
    std::string& name, std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort)
    : name_(getQString(name)), ipAddress_(getQString(ipAddress)), serverPort_(serverPort),
    clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string&& name, std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort)
    : name_(getQString(name)), ipAddress_(getQString(ipAddress)), serverPort_(serverPort),
    clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(std::shared_ptr<Companion> companion)
    : companion_(companion), name_(getQString(companion->getName())),
    ipAddress_(getQString(companion->getSocketInfo()->getIpAddress())),
    serverPort_(companion->getSocketInfo()->getServerPort()),
    clientPort_(companion->getSocketInfo()->getClientPort())
{
    initializeFields();
}

QString SocketInfoWidget::getName() const
{
    return name_;
}

QString SocketInfoWidget::getIpAddress() const
{
    return ipAddress_;
}

uint16_t SocketInfoWidget::getServerPort() const
{
    return serverPort_;
}

uint16_t SocketInfoWidget::getClientPort() const
{
    return clientPort_;
}

bool SocketInfoWidget::isStub()
{
    return false;
}

bool SocketInfoWidget::isSelected()
{
    return isSelected_;
}

void SocketInfoWidget::select()
{
    isSelected_ = true;
    changeColor(selectedColor_);
}

void SocketInfoWidget::unselect()
{
    isSelected_ = false;
    changeColor(unselectedColor_);
}

void SocketInfoWidget::update()
{
    name_ = getQString(companion_->getName());
    nameLabel_->setText(name_);

    ipAddress_ = getQString(companion_->getSocketIpAddress());

    ipAddressLabel_->setText(ipAddress_);
    clientPort_ = companion_->getSocketClientPort();

    clientPortLabel_->setText(getQString(std::to_string(clientPort_)));
}

void SocketInfoWidget::setNewMessagesIndicatorOn()
{
    newMessagesIndicator_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff()
{
    newMessagesIndicator_->setOff();
}

void SocketInfoWidget::requestHistoryFromCompanionAction()
{
    getManager()->requestHistoryFromCompanion(companion_);
}

void SocketInfoWidget::updateCompanionAction()
{
    getGraphicManager()->updateCompanion(companion_);
}

void SocketInfoWidget::clearHistoryAction()
{
    getGraphicManager()->clearCompanionHistory(companion_);
}

void SocketInfoWidget::deleteCompanionAction()
{
    getGraphicManager()->deleteCompanion(companion_);
}

void SocketInfoWidget::clientAction()
{
    bool result = false;

    // auto companion = getManager()->getMappedCompanionBySocketInfoBaseWidget(shared_from_this());
    auto companion = getManager()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    QString currentText = connectButton_->text();

    if (isConnected_) {
        result = companion->disconnectClient();
    }
    else {
        result = companion->connectClient();
        getManager()->sendUnsentMessages(companion);
    }

    if (result) {
        // change value
        isConnected_ = !(isConnected_);

        // change connect button text
        QString nextText = getNextConnectButtonLabel(currentText);
        connectButton_->setText(nextText);

        // change indicator color
        connectionStateIndicator_->toggle();

        // set context menu action enabled
        requestHistoryAction_->setDisabled(requestHistoryAction_->isEnabled());
    }
}

void SocketInfoWidget::initializeFields()
{
    isSelected_ = false;
    isConnected_ = false;

    selectedColor_ = QColor(QColorConstants::DarkGray);
    unselectedColor_ = QColor(QColorConstants::Gray);
    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Window, unselectedColor_);
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_ = std::make_unique<QHBoxLayout>();
    setLayout(layout_.get());
    connectionStateIndicator_ = std::make_unique<IndicatorWidget>(15, false);
    nameLabel_ = std::make_unique<QLabel>(name_);
    ipAddressLabel_ = std::make_unique<QLabel>(ipAddress_);

    QString serverPortQString = getQString(std::to_string(serverPort_));
    QString clientPortQString = getQString(std::to_string(clientPort_));

    serverPortLabel_ = std::make_unique<QLabel>(serverPortQString);
    clientPortLabel_ = std::make_unique<QLabel>(clientPortQString);
    editButton_ = std::make_unique<QPushButton>("Edit");
    connectButton_ = std::make_unique<QPushButton>(getInitialConnectButtonLabel());

    connect(
        editButton_.get(), &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        connectButton_.get(), &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction, Qt::QueuedConnection);

    if (name_ == "me") {  // TODO ???
        connectionStateIndicator_->setMe();
        editButton_->hide();
        connectButton_->hide();
    }

    newMessagesIndicator_ = std::make_unique<IndicatorWidget>(7, false);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    std::initializer_list<QWidget *> widgets {
        connectionStateIndicator_.get(), nameLabel_.get(), ipAddressLabel_.get(),
        serverPortLabel_.get(), clientPortLabel_.get(), editButton_.get(), connectButton_.get(),
        newMessagesIndicator_.get()
    };

    for (auto& widget : widgets)
        layout_->addWidget(widget);

    requestHistoryAction_ = std::make_shared<QAction>("Request chat history from companion", this);
    requestHistoryAction_->setDisabled(true);

    connect(
        requestHistoryAction_.get(), &QAction::triggered,
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
    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Window, color);

    setAutoFillBackground(true);
    setPalette(*palette_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent * event)
{
    auto manager = getManager();

    // auto baseObject = dynamic_pointer_cast<SocketInfoBaseWidget>(shared_from_this());
    auto baseObject = qobject_cast<SocketInfoBaseWidget *>(this);

    auto newCompanion = manager->getMappedCompanionBySocketInfoBaseWidget(baseObject);
    manager->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent *event) {}

void SocketInfoWidget::customMenuRequestedSlot(QPoint position)
{
    auto menu = std::make_shared<QMenu>(this);

    menu->addAction(requestHistoryAction_.get());

    auto clearHistoryAction = std::make_shared<QAction>("Clear chat history", this);
    menu->addAction(clearHistoryAction.get());

    connect(
        clearHistoryAction.get(), &QAction::triggered,
        this, &SocketInfoWidget::clearHistoryAction, Qt::QueuedConnection);

    auto deleteCompanionAction = std::make_shared<QAction>("Delete companion", this);
    menu->addAction(deleteCompanionAction.get());

    connect(
        deleteCompanionAction.get(), &QAction::triggered,
        this, &SocketInfoWidget::deleteCompanionAction, Qt::QueuedConnection);

    menu->popup(mapToGlobal(position));
}

SocketInfoStubWidget::SocketInfoStubWidget()
{
    mark_ = getQString(socketInfoStubWidget);

    layout_ = std::make_unique<QHBoxLayout>();
    setLayout(layout_.get());

    markLabel_ = std::make_unique<QLabel>(mark_);
    layout_->addWidget(markLabel_.get());
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

ShowHideWidget::ShowHideWidget()
{
    show_ = true;
    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignCenter);
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>();
    label_->setText("Show / Hide");

    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Window, QColor(showHideWidgetBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);

    layout_->addWidget(label_.get());
}

void ShowHideWidget::hideInfo()
{
    getGraphicManager()->hideInfo();
}

void ShowHideWidget::showInfo()
{
    getGraphicManager()->showInfo();
}

void ShowHideWidget::mousePressEvent(QMouseEvent * event)
{
    (show_) ? hideInfo() : showInfo();
    show_ = !(show_);
}

// void ScrollArea::wheelEvent(std::shared_ptr<QWheelEvent> event)
// {
//     logArgs("ScrollArea::wheelEvent");
// }

WidgetGroup::WidgetGroup(std::shared_ptr<Companion> companion)
    : companion_(companion), antecedentMessagesCounterMutex_(std::mutex())
{
    antecedentMessagesCounter_ = 0;
    auto graphicManager = getGraphicManager();
    auto socketInfo = companion_->getSocketInfo();
    auto widget = std::make_shared<SocketInfoWidget>(companion_);
    socketInfoBase_ = dynamic_pointer_cast<SocketInfoBaseWidget>(widget);
    graphicManager->addWidgetToCompanionPanel(socketInfoBase_);

    centralPanel_ = std::make_shared<CentralPanelWidget>(
        getGraphicManager()->getMainWindow(), companion_->getName());

    // centralPanel_ = new CentralPanelWidget(nullptr, companion_->getName());

    centralPanel_->set(companion_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, centralPanel_);

    centralPanel_->hide();
}

WidgetGroup::~WidgetGroup()
{
    getGraphicManager()->removeWidgetFromCompanionPanel(socketInfoBase_);
    hideCentralPanel();
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
    std::shared_ptr<Message> message, std::shared_ptr<MessageState> messageState)
{
    centralPanel_->addMessageWidgetToChatHistory(
        shared_from_this(), companion_, message, messageState);
}

void WidgetGroup::clearChatHistory()
{
    centralPanel_->clearChatHistory();

    auto cast = dynamic_pointer_cast<SocketInfoWidget>(socketInfoBase_);

    if (cast)
        cast->setNewMessagesIndicatorOff();

    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);
    antecedentMessagesCounter_ = 0;
}

void WidgetGroup::hideCentralPanel()
{
    centralPanel_->hide();
}

void WidgetGroup::showCentralPanel()
{
    centralPanel_->show();
    centralPanel_->scrollDownChatHistory();
}

std::shared_ptr<SocketInfoBaseWidget> WidgetGroup::getSocketInfoBase()
{
    return socketInfoBase_;
}

void WidgetGroup::sortChatHistoryElements()
{
    centralPanel_->sortChatHistoryElements(true);
}

void WidgetGroup::messageAdded()
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    auto cast = dynamic_pointer_cast<SocketInfoWidget>(socketInfoBase_);

    if (cast && antecedentMessagesCounter_ > 0)
        cast->setNewMessagesIndicatorOn();
}

void WidgetGroup::askUserForHistorySendingConfirmation()
{
    auto action = std::make_unique<CompanionAction>(ChatActionType::SEND_HISTORY, companion_);
    action->set();
}

void WidgetGroup::messageWidgetSelected(std::shared_ptr<MessageWidget> messageWidget)
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    auto messageState = companion_->getMappedMessageStateByMessageWidget(true, messageWidget);
    bool isAntecedent = messageState->isAntecedent();

    logArgs("isAntecedent:", isAntecedent);

    if (isAntecedent) {
        if (antecedentMessagesCounter_ != 0)
            --antecedentMessagesCounter_;

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);

        messageState->setIsAntecedent(false);

        if (antecedentMessagesCounter_ == 0) {
            auto cast = dynamic_pointer_cast<SocketInfoWidget>(socketInfoBase_);

            if (cast)
                cast->setNewMessagesIndicatorOff();
        }
    }
}

void WidgetGroup::buildChatHistorySlot()
{
    companion_->addMessageWidgetsToChatHistory();
}

void WidgetGroup::addMessageWidgetToCentralPanelChatHistorySlot(
    std::shared_ptr<MessageState> messageState, std::shared_ptr<Message> message)
{
    bool isAntecedent = messageState->isAntecedent();

    if (isAntecedent) {
        std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);
        ++antecedentMessagesCounter_;
        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);
    }

    centralPanel_->addMessageWidgetToChatHistory(
        shared_from_this(), companion_, message, messageState);
}

void WidgetGroup::askUserForHistorySendingConfirmationSlot()
{
    askUserForHistorySendingConfirmation();
}

StubWidgetGroup::StubWidgetGroup()
{
    socketInfo_ = std::make_shared<SocketInfoStubWidget>();
    leftPanel_ = std::make_shared<LeftPanelWidget>(nullptr);
    centralPanel_ = std::make_shared<CentralPanelWidget>(nullptr, "");
    rightPanel_ = std::make_shared<RightPanelWidget>(nullptr);
}

void StubWidgetGroup::set()
{
    auto graphicManager = getGraphicManager();
    auto baseObjectCast = dynamic_pointer_cast<SocketInfoBaseWidget>(socketInfo_);
    graphicManager->addWidgetToCompanionPanel(baseObjectCast);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, leftPanel_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, centralPanel_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::RIGHT, rightPanel_);

    leftPanel_->hide();
    centralPanel_->hide();
    rightPanel_->hide();
}

void StubWidgetGroup::setParents(
    std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer)
{
    leftPanel_->setParent(centralContainer.get());
    centralPanel_->setParent(centralContainer.get());
    rightPanel_->setParent(centralContainer.get());
}

void StubWidgetGroup::hideSocketInfoStubWidget()
{
    socketInfo_->hide();
}

void StubWidgetGroup::hideCentralPanel()
{
    centralPanel_->hide();
}

void StubWidgetGroup::showCentralPanel()
{
    centralPanel_->show();
}

void StubWidgetGroup::hideStubPanels()
{
    leftPanel_->hide();

    if (getManager()->getSelectedCompanion())
        centralPanel_->hide();

    rightPanel_->hide();
}

void StubWidgetGroup::showStubPanels()
{
    leftPanel_->show();
    centralPanel_->show();
    rightPanel_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width)
{
    leftPanel_->resize(width, leftPanel_->height());
}

MainWindowContainerWidget::MainWindowContainerWidget(std::shared_ptr<QWidget> widget)
{
    if (widget)
        setParent(widget.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());
}

void MainWindowContainerWidget::addWidgetToLayout(std::shared_ptr<QWidget> widget)
{
    if (widget)
        layout_->addWidget(widget.get());
}

void MainWindowContainerWidget::addWidgetToLayoutAndSetParentTo(std::shared_ptr<QWidget> widget)
{
    if (widget) {
        layout_->addWidget(widget.get());
        widget->setParent(this);
    }
}
