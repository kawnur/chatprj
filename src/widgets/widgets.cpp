#include "widgets.hpp"

#include "action.hpp"
#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "graphic_manager.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils_widgets.hpp"
#include "widgets_message.hpp"
#include "widgets_panel.hpp"

ButtonType getInitialConnectButtonType()
{
    return runAndReturnDefault([]() { return CONNECT_BUTTONS.at(0); }, ButtonType::UNKNOWN);
}

ButtonType getNextConnectButtonType(ButtonType currentValue)
{
    auto lambda = [&](const auto &container) {
        auto current = std::ranges::find(container, currentValue);

        if (current == container.end()) {
            auto entry = getStringByFormat("current type '{}' not found in container", currentValue);

            throw std::runtime_error(entry);
        }

        auto next = current + 1;

        if (next == container.end())
            next = container.begin();

        return *next;
    };

    return runAndReturnDefault(lambda, ButtonType::UNKNOWN, CONNECT_BUTTONS);
}

// Button::Button(ButtonType type) : type_(type), QPushButton(getButtonText(type)) {}

// Button::~Button() { logArgsInfo(Q_FUNC_INFO); }

ButtonInfo::ButtonInfo(): type_(ButtonType::UNKNOWN), function_() {}

ButtonInfo::ButtonInfo(ButtonType type, std::function<void()> function)
    : type_(type), function_(function) {}

ButtonInfo::~ButtonInfo() { logArgsInfo(Q_FUNC_INFO); }

ButtonType ButtonInfo::type() const
{
    return type_;
}

std::function<void()> ButtonInfo::function() const
{
    return function_;
}

ButtonBox::ButtonBox(const std::initializer_list<ButtonInfo> &list) : QDialogButtonBox()
{
    for (const auto &info : list) {
        auto type = info.type();
        auto button = addButton(getButtonText(type), getButtonRole(type));

        connect(button, &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

ButtonBox::~ButtonBox() { logArgsInfo(Q_FUNC_INFO); }

TextEditWidget::TextEditWidget()
{
    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Base, QColor(getWidgetColor(Widget::TEXT_EDIT)));
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
            sendSlot();
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

void TextEditWidget::sendSlot()
{
    auto text = toPlainText();
    setText("");
    emit sendSignal(text);
}

IndicatorWidget::IndicatorWidget(uint8_t size, bool isOn)
{
    size_ = size;
    setFixedWidth(size_);
    setFixedHeight(size_);

    isOn_ = isOn;
    onColor_ = QColor(QColorConstants::DarkGreen);
    offColor_ = QColor(QColorConstants::DarkRed);
    meColor_ = QColor(getWidgetColor(Widget::INDICATOR_ME));

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

SocketInfoWidget::SocketInfoWidget(CompanionPtr companion)
{
    buildFields();

    companion_ = companion;

    setFieldsFromCompanionData();
    setFields();
}

QString SocketInfoWidget::name() const
{
    return name_->text();
}

QString SocketInfoWidget::ipAddress() const
{
    return ipAddress_->text();
}

QString SocketInfoWidget::serverPort() const
{
    return serverPort_->text();
}

QString SocketInfoWidget::clientPort() const
{
    return clientPort_->text();
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
    changeColor(selected_);
}

void SocketInfoWidget::unselect()
{
    isSelected_ = false;
    changeColor(unselected_);
}

void SocketInfoWidget::updateFields()
{
    setFieldsFromCompanionData();
}

void SocketInfoWidget::setNewMessagesIndicatorOn()
{
    received_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff()
{
    received_->setOff();
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
    auto manager = getManager();
    auto companion = getGraphicManager()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    if (isConnected_) {
        result = manager->disconnectClient(companion);
    }
    else {
        result = manager->connectClient(companion);
        getManager()->sendUnsentMessages(companion);
    }

    if (!result)
        return;

    // change value
    isConnected_ = !(isConnected_);

    // change connect button text
    connectButtonType_ = getNextConnectButtonType(connectButtonType_);
    connect_->setText(getButtonText(connectButtonType_));

    // change indicator color
    state_->toggle();

    // set context menu action enabled
    requestHistory_->setDisabled(requestHistory_->isEnabled());
}

void SocketInfoWidget::buildFields()
{
    // value fields
    isSelected_ = false;
    isConnected_ = false;
    selected_ = QColor(QColorConstants::DarkGray);
    unselected_ = QColor(QColorConstants::Gray);

    // palette
    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, unselected_);
    setAutoFillBackground(true);
    setPalette(*palette_);

    // layout
    layout_ = std::make_shared<QHBoxLayout>();
    setLayout(layout_.get());

    // widget fields
    state_ = std::make_shared<IndicatorWidget>(CONNECTION_STATE_INDICATOR_WIDGET_SIZE, false);

    name_ = std::make_shared<QLabel>();
    ipAddress_ = std::make_shared<QLabel>();
    serverPort_ = std::make_shared<QLabel>();
    clientPort_ = std::make_shared<QLabel>();
    edit_ = std::make_shared<QPushButton>(getButtonText(ButtonType::EDIT));
    connectButtonType_ = getInitialConnectButtonType();
    connect_ = std::make_shared<QPushButton>(getButtonText(connectButtonType_));

    connect(
        edit_.get(), &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        connect_.get(), &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction, Qt::QueuedConnection);

    received_ = std::make_shared<IndicatorWidget>(NEW_MESSAGES_INDICATOR_WIDGET_SIZE, false);

    // add widgets to layout
    std::vector<QWidgetPtr> widgets {
        state_, name_, ipAddress_, serverPort_, clientPort_, edit_, connect_, received_
    };

    for (auto &widget : widgets)
        layout_->addWidget(widget.get());

    // ???
    requestHistory_ = std::make_shared<QAction>("Request chat history from companion", this);
    requestHistory_->setDisabled(true);

    // connections
    connect(
        requestHistory_.get(), &QAction::triggered,
        this, &SocketInfoWidget::requestHistoryFromCompanionAction,
        Qt::QueuedConnection);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, &QWidget::customContextMenuRequested,
        this, &SocketInfoWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void SocketInfoWidget::setFields()
{
    if (name_->text() == ME_NAME) {  // TODO ???
        state_->setMe();
        edit_->hide();
        connect_->hide();
    }
}

void SocketInfoWidget::setFieldsFromCompanionData()
{
    name_->setText(getQString(companion_->name()));
    auto data = getManager()->getCompanionData(companion_);

    if (data) {
        ipAddress_->setText(getQString(data->ipAddress()));
        serverPort_->setText(getQString(data->serverPort()));
        clientPort_->setText(getQString(data->clientPort()));
    }
}

void SocketInfoWidget::changeColor(QColor &color)
{
    palette_ = std::make_shared<QPalette>();
    palette_->setColor(QPalette::Window, color);

    setAutoFillBackground(true);
    setPalette(*palette_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent *event)
{
    auto manager = getManager();

    auto baseObject = qobject_cast<SocketInfoBaseWidget *>(this);

    auto newCompanion = getGraphicManager()->getMappedCompanionBySocketInfoBaseWidget(baseObject);
    manager->resetSelectedCompanion(newCompanion);

    QWidget::mousePressEvent(event);
}

void SocketInfoWidget::customMenuRequestedSlot(QPoint position)
{
    auto menu = new Menu();

    menu->addAction(requestHistory_.get());

    auto clearHistoryAction = new QAction("Clear chat history", this);
    menu->addAction(clearHistoryAction);

    connect(
        clearHistoryAction, &QAction::triggered,
        this, &SocketInfoWidget::clearHistoryAction, Qt::QueuedConnection);

    auto deleteCompanionAction = new QAction("Delete companion", this);
    menu->addAction(deleteCompanionAction);

    connect(
        deleteCompanionAction, &QAction::triggered,
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
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(Widget::SHOW_HIDE)));
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

void ShowHideWidget::mousePressEvent(QMouseEvent *event)
{
    (show_) ? hideInfo() : showInfo();
    show_ = !(show_);
}

// void ScrollArea::wheelEvent(std::shared_ptr<QWheelEvent> event)
// {
//     logArgs(__FUNCTION__);
// }

WidgetGroup::WidgetGroup(CompanionPtr companion)
    : companion_(companion), antecedentMessagesCounterMutex_(std::mutex())
{
    antecedentMessagesCounter_ = 0;
    auto graphicManager = getGraphicManager();
    auto widget = std::make_shared<SocketInfoWidget>(companion_);
    socketInfoBase_ = dynamic_pointer_cast<SocketInfoBaseWidget>(widget);
    graphicManager->addCompanionWidget(socketInfoBase_);

    centralPanel_ = std::make_shared<CentralPanelWidget>(
        getGraphicManager()->getMainWindow(), companion_->name());

    centralPanel_->set(companion_);

    graphicManager->addWidgetToMainWindowContainerAndSetParentTo(
        MWContainerPosition::CENTRAL, centralPanel_);

    centralPanel_->hide();
}

WidgetGroup::~WidgetGroup()
{
    getGraphicManager()->removeCompanionWidget(socketInfoBase_);
    hideCentralPanel();
}

void WidgetGroup::set()
{
    connect(
        this, &WidgetGroup::addMessageWidgetSignal,
        this, &WidgetGroup::addMessageWidgetSlot,
        Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::askUserForHistorySendingConfirmationSignal,
        this, &WidgetGroup::askUserForHistorySendingConfirmationSlot,
        Qt::QueuedConnection);

    // connect(
    //     this, &WidgetGroup::buildChatHistorySignal,
    //     this, &WidgetGroup::buildChatHistorySlot,
    //     Qt::QueuedConnection);
}

void WidgetGroup::clearChatHistory()
{
    centralPanel_->clearHistory();

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
    centralPanel_->scrollDownHistory();
}

SocketInfoBaseWidgetPtr WidgetGroup::getSocketInfoBase()
{
    return socketInfoBase_;
}

void WidgetGroup::sortChatHistoryElements()
{
    centralPanel_->sortHistory(true);
}

void WidgetGroup::messageAdded()
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    if (antecedentMessagesCounter_ <= 0)
        return;

    auto cast = dynamic_pointer_cast<SocketInfoWidget>(socketInfoBase_);

    if (cast)
        cast->setNewMessagesIndicatorOn();
}

void WidgetGroup::askUserForHistorySendingConfirmation()
{
    auto action = std::make_unique<CompanionAction>(ActionType::SEND_HISTORY, companion_);
    action->set();
}

void WidgetGroup::updateSocketInfoWidget()
{
    socketInfoBase_->updateFields();
}

void WidgetGroup::messageWidgetSelected(MessageWidgetPtr widget)
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    auto messageState = widget->message()->state();

    if (!messageState)
        return;

    bool isAntecedent = messageState->isAntecedent_;

    logArgs("isAntecedent:", isAntecedent);

    if (isAntecedent) {
        if (antecedentMessagesCounter_ != 0)
            --antecedentMessagesCounter_;

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);

        messageState->isAntecedent_ = false;

        if (antecedentMessagesCounter_ == 0) {
            auto cast = dynamic_pointer_cast<SocketInfoWidget>(socketInfoBase_);

            if (cast)
                cast->setNewMessagesIndicatorOff();
        }
    }
}

// void WidgetGroup::buildChatHistorySlot()
// {
//     companion_->addMessageWidgetsToChatHistory();
// }

void WidgetGroup::addMessageWidgetSlot(MessagePtr message)
{
    bool isAntecedent = message->isAntecedent();

    if (isAntecedent) {
        std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

        ++antecedentMessagesCounter_;

        logArgs("antecedentMessagesCounter_:", antecedentMessagesCounter_);
    }

    auto widget = getGraphicManager()->buildMessageWidget(companion_, message);

    // auto lambda = [=]()
    // {
    //     // companion->setMappedMessageWidget(message, widget);
    //     getManager()->setMappedMessageWidget(companion_, message, widget);
    // };

    // runInDetachedThread(lambda);
    getManager()->setMappedMessageWidget(companion_, message, widget);

    widget->setBase(shared_from_this());

    centralPanel_->addMessage(widget, isAntecedent);

    messageAdded();
}

void WidgetGroup::askUserForHistorySendingConfirmationSlot()
{
    askUserForHistorySendingConfirmation();
}

StubWidgetGroup::StubWidgetGroup()
{
    socketInfo_ = std::make_shared<SocketInfoStubWidget>();
    left_ = std::make_shared<LeftPanelWidget>(nullptr);
    central_ = std::make_shared<CentralPanelWidget>(nullptr, "");
    right_ = std::make_shared<RightPanelWidget>(nullptr);
}

void StubWidgetGroup::hide()
{
    left_->hide();
    central_->hide();
    right_->hide();
}

void StubWidgetGroup::show()
{
    left_->show();
    central_->show();
    right_->show();
}

void StubWidgetGroup::setParents(QWidgetPtr leftContainer, QWidgetPtr centralContainer)
{
    left_->setParent(centralContainer.get());
    central_->setParent(centralContainer.get());
    right_->setParent(centralContainer.get());
}

void StubWidgetGroup::hideSocketInfo()
{
    socketInfo_->hide();
}

void StubWidgetGroup::hideCentral()
{
    central_->hide();
}

void StubWidgetGroup::showCentral()
{
    central_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width)
{
    left_->resize(width, left_->height());
}

std::shared_ptr<SocketInfoBaseWidget> StubWidgetGroup::socketInfoBase() const
{
    return dynamic_pointer_cast<SocketInfoBaseWidget>(socketInfo_);
}

std::shared_ptr<LeftPanelWidget> StubWidgetGroup::left() const
{
    return left_;
}

std::shared_ptr<CentralPanelWidget> StubWidgetGroup::central() const
{
    return central_;
}

std::shared_ptr<RightPanelWidget> StubWidgetGroup::right() const
{
    return right_;
}

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object)
{
    logTemplateInfo(
        "name: {0}, ipAddress: {1}, serverPort_: {2}, clientPort_: {3}",
        object->name().toStdString(), object->ipAddress().toStdString(),
        object->serverPort(), object->clientPort());
}
