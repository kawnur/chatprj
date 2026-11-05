#include "widgets.hpp"

#include <QSizePolicy>

#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "graphic_manager.hpp"
#include "mainwindow.hpp"
#include "message.hpp"
#include "utils_widgets.hpp"
#include "widgets_message.hpp"
#include "widgets_panel.hpp"

Palette::Palette(WidgetType type, QPalette::ColorRole role) : type_(type), role_(role)
{
    palette_ = std::make_unique<QPalette>();
    setColor(getWidgetColor(type_));
}

WidgetType Palette::type() const
{
    return type_;
}

void Palette::setType(WidgetType type)
{
    type_ = type;
    setColor(getWidgetColor(type_));
}

void Palette::setColor(uint32_t color)
{
    palette_->setColor(role_, QColor(color));
}

PalettedWidget::PalettedWidget(WidgetType type) : QWidget(), Palette(type, QPalette::Window)
{
    setAutoFillBackground(true);
    setPalette(*palette_);
}

void PalettedWidget::updateType(WidgetType type)
{
    setType(type);
    setPalette(*palette_);
}

void PalettedWidget::updateColor(uint32_t color)
{
    setColor(color);
    setPalette(*palette_);
}

Layout::Layout(WidgetType type) : type_(type)
{
    spacing_ = getLayoutSpacingValue(type_);
    leftMargin_ = getLayoutContentsLeftMargin(type);
    topMargin_ = getLayoutContentsTopMargin(type);
    rightMargin_ = getLayoutContentsRightMargin(type);
    bottomMargin_ = getLayoutContentsBottomMargin(type);
}

VLayout::VLayout(WidgetType type, Qt::AlignmentFlag alignment)
    : Layout(type), layout_(new QVBoxLayout)
{
    layout_->setSpacing(spacing_);
    layout_->setContentsMargins(leftMargin_, topMargin_, rightMargin_, bottomMargin_);

    if (alignment == Qt::AlignCenter)
        layout_->setAlignment(alignment);
    else
        layout_->setAlignment(alignment | Qt::AlignTop);

    layout_->setSizeConstraint(QLayout::SetMaximumSize);
}

void VLayout::addWidget(QWidget *widget)
{
    layout_->addWidget(widget);
}

void VLayout::removeWidget(QWidget *widget)
{
    layout_->removeWidget(widget);
}

void VLayout::logLayout()
{
    logArgsInfo(LAYOUT_LOG_DELIMITER);

    for (std::size_t i = 0; i < layout_->count(); ++i)
        logArgsInfo(layout_->itemAt(i)->widget());

    logArgsInfo(LAYOUT_LOG_DELIMITER);
}

HLayout::HLayout(WidgetType type, Qt::AlignmentFlag alignment)
    : Layout(type), layout_(new QHBoxLayout)
{
    layout_->setSpacing(spacing_);
    layout_->setContentsMargins(leftMargin_, topMargin_, rightMargin_, bottomMargin_);
    layout_->setAlignment(alignment | Qt::AlignTop);
    layout_->setSizeConstraint(QLayout::SetMaximumSize);
}

void HLayout::addWidget(QWidget *widget)
{
    layout_->addWidget(widget);
}

void HLayout::removeWidget(QWidget *widget)
{
    layout_->removeWidget(widget);
}

void HLayout::logLayout()
{
    logArgsInfo(LAYOUT_LOG_DELIMITER);

    for (std::size_t i = 0; i < layout_->count(); ++i)
        logArgsInfo(layout_->itemAt(i)->widget());

    logArgsInfo(LAYOUT_LOG_DELIMITER);
}

VLayoutWidget::VLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment)
    : PalettedWidget(type), VLayout(type, alignment)
{
    setLayout(layout_.get());
}

HLayoutWidget::HLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment)
    : PalettedWidget(type), HLayout(type, alignment)
{
    setLayout(layout_.get());
}

LabelWidget::LabelWidget(WidgetType type, const QString &text)
    : QLabel(text), Palette(type, QPalette::Window)
{
    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

SplitterWidget::SplitterWidget(WidgetType type, QWidget *parent) : Palette(type, QPalette::Window)
{
    if (parent)
        setParent(parent);

    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
}

TextEditWidget::TextEditWidget(WidgetType type) : Palette(type, QPalette::Base)
{
    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
}

Button::Button(ButtonType type) : type_(type), QPushButton(getButtonText(type)) {}

Button::~Button() { logArgsInfo(Q_FUNC_INFO); }

StateButton::StateButton(ButtonType type) : Button(type), state_(ButtonState::UNKNOWN)
{
    if (!BUTTON_TYPE_TO_STATE_LIST_MAP.contains(type_))
        return;

    auto list = BUTTON_TYPE_TO_STATE_LIST_MAP.at(type_);

    if (list.empty())
        return;

    state_ = list.front();
    setText(getButtonText(state_));
}

void StateButton::updateState()
{
    if (!BUTTON_TYPE_TO_STATE_LIST_MAP.contains(type_))
        return;

    state_ = getNextItem(BUTTON_TYPE_TO_STATE_LIST_MAP.at(type_), state_, ButtonState::UNKNOWN);
    setText(getButtonText(state_));
}

ButtonInfo::ButtonInfo(): isStateButton_(false), type_(ButtonType::UNKNOWN), function_() {}

ButtonInfo::ButtonInfo(bool isStateButton, ButtonType type, std::function<void()> function)
    : isStateButton_(isStateButton), type_(type), function_(function) {}

ButtonInfo::~ButtonInfo() { logArgsInfo(Q_FUNC_INFO); }

bool ButtonInfo::isStateButton() const
{
    return isStateButton_;
}

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

MessageEditWidget::MessageEditWidget(WidgetType type) : TextEditWidget(type) {}

void MessageEditWidget::keyPressEvent(QKeyEvent *event)
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

void MessageEditWidget::sendSlot()
{
    auto text = toPlainText();
    setText("");
    emit sendSignal(text);
}

IndicatorWidget::IndicatorWidget(WidgetType type, IndicatorWidgetState state)
    : PalettedWidget(type)
{
    size_ = getIndicatorSize(type_);
    setFixedWidth(size_);
    setFixedHeight(size_);
    state_ = state;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::setOn()
{
    state_ = IndicatorWidgetState::ON;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::setOff()
{
    state_ = IndicatorWidgetState::OFF;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::updateState()
{
    if (state_ == IndicatorWidgetState::OFF)
        state_ = IndicatorWidgetState::ON;
    else if (state_ == IndicatorWidgetState::ON)
        state_ = IndicatorWidgetState::OFF;

    updateColor(getIndicatorWidgetColor(state_));
}

ButtonPanelWidget::ButtonPanelWidget(WidgetType type, const std::initializer_list<ButtonInfo> &list)
    : HLayoutWidget(type, Qt::AlignLeft), buttons_()
{
    for (const auto &info : list) {
        auto type = info.type();

        std::shared_ptr<Button> button = nullptr;

        if (info.isStateButton())
            button = std::make_shared<StateButton>(type);
        else
            button = std::make_shared<Button>(type);

        buttons_[type] = button;
        addWidget(button.get());

        connect(button.get(), &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

void ButtonPanelWidget::updateButtonState(ButtonType type)
{
    runAndLogException([&]() { buttons_.at(type)->updateState(); });
}

SocketInfoBaseWidget::SocketInfoBaseWidget()
    : HLayoutWidget(WidgetType::SOCKET_INFO, Qt::AlignLeft) {}

SocketInfoWidget::SocketInfoWidget(CompanionPtr companion)
    : isSelected_(false), isConnected_(false), companion_(companion) {}

void SocketInfoWidget::set(WidgetGroupPtr group)
{
    // size
    setFixedWidth(SOCKET_INFO_WIDGET_WIDTH);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // widget fields
    IndicatorWidgetState state = IndicatorWidgetState::OFF;

    if (companion_->name() == ME_NAME) {
        updateType(WidgetType::SOCKET_INFO_UNKNOWN);
        state = IndicatorWidgetState::UNKNOWN;
    }

    state_ = std::make_shared<IndicatorWidget>(WidgetType::CONNECTION_STATE_INDICATOR, state);

    name_ = std::make_shared<QLabel>();
    ipAddress_ = std::make_shared<QLabel>();
    serverPort_ = std::make_shared<QLabel>();
    clientPort_ = std::make_shared<QLabel>();

    // buttons
    std::initializer_list<ButtonInfo> list = {
        { false, ButtonType::EDIT, [this]() { updateCompanionAction(); } },
        { true, ButtonType::CONNECT_DISCONNECT, [=, this]() { group->clientAction(); } }
    };

    buttons_ = std::make_shared<ButtonPanelWidget>(WidgetType::SOCKET_INFO_BUTTON_PANEL, list);
    received_ = std::make_shared<IndicatorWidget>(WidgetType::NEW_MESSAGES_INDICATOR, state);

    // add to layout
    std::vector<QWidgetPtr> widgets1 { state_, name_, ipAddress_, serverPort_, clientPort_ };
    std::vector<QWidgetPtr> widgets2 { buttons_, received_ };

    for (auto &widget : widgets1)
        addWidget(widget.get());

    layout_->addStretch(1);

    for (auto &widget : widgets2)
        addWidget(widget.get());

    // context menu
    requestHistory_ = std::make_shared<QAction>("Request chat history from companion", this);
    requestHistory_->setDisabled(true);

    connect(
        requestHistory_.get(), &QAction::triggered,
        group.get(), &WidgetGroup::requestHistorySlot, Qt::QueuedConnection);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, &QWidget::customContextMenuRequested,
        this, &SocketInfoWidget::customMenuRequestedSlot, Qt::QueuedConnection);

    setValues();
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

bool SocketInfoWidget::isConnected()
{
    return isConnected_;
}

void SocketInfoWidget::select()
{
    isSelected_ = true;
    updateType(WidgetType::SOCKET_INFO_SELECTED);
    buttons_->updateType(WidgetType::SOCKET_INFO_BUTTON_PANEL_SELECTED);
}

void SocketInfoWidget::unselect()
{
    isSelected_ = false;
    updateType(WidgetType::SOCKET_INFO);
    buttons_->updateType(WidgetType::SOCKET_INFO_BUTTON_PANEL);
}

void SocketInfoWidget::updateFields()
{
    setFieldsFromCompanionData();
}

// void SocketInfoWidget::updateNewMessagesIndicator()
// {
//     received_->updateState();
// }

void SocketInfoWidget::setNewMessagesIndicatorOn()
{
    received_->setOn();
}

void SocketInfoWidget::setNewMessagesIndicatorOff()
{
    received_->setOff();
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

void SocketInfoWidget::updateConnectionState()
{
    // change value
    isConnected_ = !(isConnected_);

    // change connect button text
    buttons_->updateButtonState(ButtonType::CONNECT_DISCONNECT);

    // change indicator color
    state_->updateState();

    // set context menu action disabled
    requestHistory_->setDisabled(requestHistory_->isEnabled());
}

void SocketInfoWidget::setValues()
{
    setFieldsFromCompanionData();

    if (type() == WidgetType::SOCKET_INFO_UNKNOWN) {
        ipAddress_->hide();
        serverPort_->hide();
        clientPort_->hide();
        buttons_->hide();
        received_->hide();
    }
}

void SocketInfoWidget::setFieldsFromCompanionData()
{
    name_->setText(getQString(companion_->name()));
    auto data = getGraphicManager()->getCompanionData(companion_);

    if (data) {
        ipAddress_->setText(getQString(data->ipAddress()));
        serverPort_->setText(getQString(data->serverPort()));
        clientPort_->setText(getQString(data->clientPort()));
    }
}

void SocketInfoWidget::mousePressEvent(QMouseEvent *event)
{
    getGraphicManager()->resetSelectedCompanion(companion_);

    QWidget::mousePressEvent(event);
}

CompanionPtr SocketInfoWidget::companion()
{
    return companion_;
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
    markLabel_ = std::make_unique<QLabel>(mark_);
    addWidget(markLabel_.get());
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

ShowHideWidget::ShowHideWidget() : VLayoutWidget(WidgetType::SHOW_HIDE, Qt::AlignCenter)
{
    show_ = true;

    label_ = std::make_unique<QLabel>();
    label_->setText("Show / Hide");

    addWidget(label_.get());
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

ScrollAreaWidget::ScrollAreaWidget(QWidgetPtr widget)
{
    setWidgetResizable(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setWidget(widget.get());
}

WidgetGroup::WidgetGroup(CompanionPtr companion)
    : companion_(companion), antecedentMessagesCounterMutex_(), antecedentMessagesCounter_(0) {}

WidgetGroup::~WidgetGroup()
{
    logArgsInfo(Q_FUNC_INFO);
    getGraphicManager()->removeCompanionWidget(socketInfoBase_);
    hideCentralPanel();
}

void WidgetGroup::set()
{
    auto graphicManager = getGraphicManager();

    auto widget = std::make_shared<SocketInfoWidget>(companion_);
    widget->set(shared_from_this());

    socketInfoBase_ = dynamic_pointer_cast<SocketInfoBaseWidget>(widget);
    graphicManager->addCompanionWidget(socketInfoBase_);

    centralPanel_ = std::make_shared<CentralPanelWidget>(companion_->name(), false);

    centralPanel_->set(companion_);
    graphicManager->addWidget(MWContainerPosition::CENTRAL, centralPanel_);
    centralPanel_->hide();

    connect(
        this, &WidgetGroup::addMessageWidgetSignal, this, &WidgetGroup::addMessageWidgetSlot,
        Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::addMessageWidgetsSignal, this, &WidgetGroup::addMessageWidgetsSlot,
        Qt::QueuedConnection);
}

void WidgetGroup::clearHistory()
{
    centralPanel_->clearHistory();
    setNewMessagesIndicatorOff();
    // socketInfoBase_->updateNewMessagesIndicator();

    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);
    antecedentMessagesCounter_ = 0;
}

void WidgetGroup::hideFields()
{
    socketInfoBase_->hide();
    centralPanel_->hide();
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

void WidgetGroup::messageAdded()
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    if (antecedentMessagesCounter_ <= 0)
        return;

    setNewMessagesIndicatorOn();
    // socketInfoBase_->updateNewMessagesIndicator();
}

void WidgetGroup::updateSocketInfoWidget()
{
    socketInfoBase_->updateFields();
}

void WidgetGroup::select()
{
    socketInfoBase_->select();
}

void WidgetGroup::unselect()
{
    socketInfoBase_->unselect();
}

void WidgetGroup::setNewMessagesIndicatorOn()
{
    socketInfoBase_->setNewMessagesIndicatorOn();
}

void WidgetGroup::setNewMessagesIndicatorOff()
{
    socketInfoBase_->setNewMessagesIndicatorOff();
}

void WidgetGroup::messageWidgetSelected(MessageWidgetPtr widget)
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    auto messageState = widget->message()->state();

    if (!messageState)
        return;

    bool isAntecedent = messageState->isAntecedent();

    logTemplateInfo("isAntecedent: {}", isAntecedent);

    if (isAntecedent) {
        if (antecedentMessagesCounter_ != 0)
            --antecedentMessagesCounter_;

        logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);

        messageState->setIsAntecedent(false);

        if (antecedentMessagesCounter_ == 0)
            setNewMessagesIndicatorOff();
            // socketInfoBase_->updateNewMessagesIndicator();
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

        logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
    }

    auto widget = getGraphicManager()->buildMessageWidget(companion_, message);

    // auto lambda = [=]()
    // {
    //     // companion->setMappedMessageWidget(message, widget);
    //     getManager()->setMappedMessageWidget(companion_, message, widget);
    // };

    // runInDetachedThread(lambda);
    auto result = getGraphicManager()->setMappedMessageWidget(companion_, message, widget);
    // widget->setBase(shared_from_this());
    widget->set(shared_from_this());
    centralPanel_->addMessage(widget, isAntecedent);
    messageAdded();
}

void WidgetGroup::addMessageWidgetsSlot(const std::vector<MessagePtr> &messages)
{
    for (auto message : messages)
        addMessageWidgetSlot(message);

    centralPanel_->sortHistory(true);
}

void WidgetGroup::requestHistorySlot()
{
    getGraphicManager()->requestHistory(companion_);
}

void WidgetGroup::clientAction()
{
    if (!getGraphicManager()->clientAction(companion_, socketInfoBase_->isConnected()))
        return;

    socketInfoBase_->updateConnectionState();
}

// void WidgetGroup::askUserForHistorySendingConfirmationSlot()
// {
//     auto action = std::make_unique<CompanionAction>(ActionType::SEND_HISTORY, companion_);
//     action->set();
// }

StubWidgetGroup::StubWidgetGroup()
{
    socketInfo_ = std::make_shared<SocketInfoStubWidget>();
    left_ = std::make_shared<LeftPanelWidget>();
    central_ = std::make_shared<CentralPanelWidget>("", true);
    right_ = std::make_shared<RightPanelWidget>();

    central_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    central_->setFixedWidth(400);
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
        "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}",
        object->name(), object->ipAddress(), object->serverPort(), object->clientPort());
}
