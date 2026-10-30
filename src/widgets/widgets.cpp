#include "widgets.hpp"

#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "graphic_manager.hpp"
#include "mainwindow.hpp"
#include "message.hpp"
#include "utils_widgets.hpp"
#include "widgets_message.hpp"
#include "widgets_panel.hpp"

PalettedWidget::PalettedWidget(WidgetType type) : type_(type)
{
    // palette_ = std::make_unique<QPalette>();
    palette_ = std::make_shared<QPalette>();
    setColor();
    setAutoFillBackground(true);
}

void PalettedWidget::setType(WidgetType type)
{
    type_ = type;
    setColor();
}

void PalettedWidget::setColor()
{
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(type_)));
    setPalette(*palette_);
}

Layout::Layout(WidgetType type) : type_(type)
{
    value_ = getLayoutValue(type_);
}

VLayout::VLayout(WidgetType type) : Layout(type), layout_(new QVBoxLayout)
{
    layout_->setSpacing(value_);
    layout_->setContentsMargins(value_, value_, value_, value_);
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
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

HLayout::HLayout(WidgetType type) : Layout(type), layout_(new QHBoxLayout)
{
    layout_->setSpacing(value_);
    layout_->setContentsMargins(value_, value_, value_, value_);
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
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

VLayoutWidget::VLayoutWidget(WidgetType type) : PalettedWidget(type), VLayout(type)
{
    setLayout(layout_.get());
}

HLayoutWidget::HLayoutWidget(WidgetType type) : PalettedWidget(type), HLayout(type)
{
    setLayout(layout_.get());
}

LabelWidget::LabelWidget(WidgetType type, const QString &text) : type_(type), QLabel(text)
{
    // palette_ = std::make_unique<QPalette>();
    palette_ = std::make_shared<QPalette>();
    setColor();
    setAutoFillBackground(true);
}

void LabelWidget::setColor()
{
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(type_)));
    setPalette(*palette_);
}

SplitterWidget::SplitterWidget(WidgetType type, QWidget *parent) : type_(type)
{
    if (parent)
        setParent(parent);

    // palette_ = std::make_unique<QPalette>();
    palette_ = std::make_shared<QPalette>();
    setColor();
    setAutoFillBackground(true);
}

void SplitterWidget::setColor()
{
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(type_)));
    setPalette(*palette_);
}

TextEditWidget::TextEditWidget(WidgetType type) : type_(type)
{
    // palette_ = std::make_unique<QPalette>();
    palette_ = std::make_shared<QPalette>();
    setColor();
    setAutoFillBackground(true);
}

void TextEditWidget::setColor()
{
    palette_->setColor(QPalette::Base, QColor(getWidgetColor(type_)));
    setPalette(*palette_);
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

IndicatorWidget::IndicatorWidget(WidgetType type, bool isOn) : PalettedWidget(type)
{
    size_ = getIndicatorSize(type_);
    setFixedWidth(size_);
    setFixedHeight(size_);

    isOn_ = isOn;
    onColor_ = QColor(getWidgetColor(WidgetType::INDICATOR_ON));
    offColor_ = QColor(getWidgetColor(WidgetType::INDICATOR_OFF));
    meColor_ = QColor(getWidgetColor(WidgetType::INDICATOR_ME));

    (isOn_) ? setOn() : setOff();
}

void IndicatorWidget::setOn()
{
    isOn_ = true;
    setType(WidgetType::INDICATOR_ON);
}

void IndicatorWidget::setOff()
{
    isOn_ = false;
    setType(WidgetType::INDICATOR_OFF);
}

void IndicatorWidget::setMe()
{
    isOn_ = false;
    setType(WidgetType::INDICATOR_ME);
}

void IndicatorWidget::toggle()
{
    (isOn_) ? setOff() : setOn();
}

ButtonPanelWidget::ButtonPanelWidget(const std::initializer_list<ButtonInfo> &list)
    : HLayoutWidget(WidgetType::BUTTON_PANEL), buttons_()
{
    for (const auto &info : list) {
        auto type = info.type();
        auto button = std::make_shared<QPushButton>(getButtonText(type));
        buttons_.push_back(button);
        addWidget(button.get());

        QObject::connect(
            button.get(), &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

// SocketInfoBaseWidget::SocketInfoBaseWidget() : PalettedWidget(WidgetType::SOCKET_INFO) {}
SocketInfoBaseWidget::SocketInfoBaseWidget() : HLayoutWidget(WidgetType::SOCKET_INFO) {}

SocketInfoWidget::SocketInfoWidget(WidgetGroupPtr group, CompanionPtr companion)
{
    buildFields(group);

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

bool SocketInfoWidget::isConnected()
{
    return isConnected_;
}

void SocketInfoWidget::select()
{
    isSelected_ = true;
    setType(WidgetType::SOCKET_INFO_SELECTED);
}

void SocketInfoWidget::unselect()
{
    isSelected_ = false;
    setType(WidgetType::SOCKET_INFO);
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

void SocketInfoWidget::updateState()
{
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

void SocketInfoWidget::buildFields(WidgetGroupPtr group)
{
    // value fields
    isSelected_ = false;
    isConnected_ = false;
    // selected_ = QColor(getWidgetColor(WidgetType::SOCKET_INFO_SELECTED));
    // unselected_ = QColor(getWidgetColor(WidgetType::SOCKET_INFO));

    // layout
    // layout_ = std::make_shared<QHBoxLayout>();
    // setLayout(layout_.get());

    // widget fields
    state_ = std::make_shared<IndicatorWidget>(WidgetType::CONNECTION_STATE_INDICATOR, false);

    name_ = std::make_shared<QLabel>();
    ipAddress_ = std::make_shared<QLabel>();
    serverPort_ = std::make_shared<QLabel>();
    clientPort_ = std::make_shared<QLabel>();
    spacer_ = std::make_shared<QSpacerItem>(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    edit_ = std::make_shared<QPushButton>(getButtonText(ButtonType::EDIT));
    connectButtonType_ = getInitialConnectButtonType();
    connect_ = std::make_shared<QPushButton>(getButtonText(connectButtonType_));

    connect(
        edit_.get(), &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        connect_.get(), &QPushButton::clicked,
        group.get(), &WidgetGroup::clientAction, Qt::QueuedConnection);

    received_ = std::make_shared<IndicatorWidget>(WidgetType::NEW_MESSAGES_INDICATOR, false);

    // add widgets to layout
    // std::vector<QWidgetPtr> widgets {
    //     state_, name_, ipAddress_, serverPort_, clientPort_, edit_, connect_, received_
    // };
    std::vector<QWidgetPtr> widgets1 {
        state_, name_, ipAddress_, serverPort_, clientPort_
    };

    std::vector<QWidgetPtr> widgets2 {
        edit_, connect_, received_
    };

    // for (auto &widget : widgets)
    //     layout_->addWidget(widget.get());
    for (auto &widget : widgets1)
        layout_->addWidget(widget.get());

    layout_->addSpacerItem(spacer_.get());

    for (auto &widget : widgets2)
        layout_->addWidget(widget.get());

    // ???
    requestHistory_ = std::make_shared<QAction>("Request chat history from companion", this);
    requestHistory_->setDisabled(true);

    // connections
    connect(
        requestHistory_.get(), &QAction::triggered,
        group.get(), &WidgetGroup::requestHistorySlot, Qt::QueuedConnection);

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

    // layout_ = std::make_unique<QHBoxLayout>();
    // setLayout(layout_.get());

    markLabel_ = std::make_unique<QLabel>(mark_);
    layout_->addWidget(markLabel_.get());
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

ShowHideWidget::ShowHideWidget() : PalettedWidget(WidgetType::SHOW_HIDE)
{
    show_ = true;
    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignCenter);
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>();
    label_->setText("Show / Hide");

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
    getGraphicManager()->removeCompanionWidget(socketInfoBase_);
    hideCentralPanel();
}

void WidgetGroup::set()
{
    auto graphicManager = getGraphicManager();

    auto widget = std::make_shared<SocketInfoWidget>(shared_from_this(), companion_);

    socketInfoBase_ = dynamic_pointer_cast<SocketInfoBaseWidget>(widget);
    graphicManager->addCompanionWidget(socketInfoBase_);

    centralPanel_ = std::make_shared<CentralPanelWidget>(
        getGraphicManager()->getMainWindow(), companion_->name());

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

void WidgetGroup::messageAdded()
{
    std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

    // set new message indicator on if socket info widget is not selected
    if (antecedentMessagesCounter_ <= 0)
        return;

    setNewMessagesIndicatorOn();
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

    socketInfoBase_->updateState();
}

// void WidgetGroup::askUserForHistorySendingConfirmationSlot()
// {
//     auto action = std::make_unique<CompanionAction>(ActionType::SEND_HISTORY, companion_);
//     action->set();
// }

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

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object)
{
    logTemplateInfo(
        "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}",
        object->name(), object->ipAddress(), object->serverPort(), object->clientPort());
}
