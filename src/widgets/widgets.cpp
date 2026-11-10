#include "widgets.hpp"

#include <cstdint>

#include <QSizePolicy>

#include "companion.hpp"
#include "data.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widget_group.hpp"

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

Button::Button(const QString &text) : QPushButton(text) {}

Button::~Button() { logArgsInfo(Q_FUNC_INFO); }

StateButton::StateButton(const std::vector<QString> &texts)
    : Button(""), texts_(std::move(texts))
{
    if (texts_.empty())
        return;

    setText(texts.at(0));
}

void StateButton::updateState()
{
    auto current = text();
    auto begin = texts_.begin();
    auto end = texts_.end();

    for (auto iter = begin; iter != end; ++iter) {
        if (*iter != current)
            continue;

        auto next = iter + 1;

        if (next == end)
            next = begin;

        setText(*next);
    }
}

ButtonInfo::ButtonInfo(): function_() {}

ButtonInfo::ButtonInfo(const std::vector<QString> &texts, std::function<void ()> function)
    : texts_(std::move(texts)), function_(function) {}

ButtonInfo::~ButtonInfo() { logArgsInfo(Q_FUNC_INFO); }

std::vector<QString> ButtonInfo::texts() const
{
    return texts_;
}

std::function<void()> ButtonInfo::function() const
{
    return function_;
}

ButtonBox::ButtonBox(const std::initializer_list<ButtonInfo> &list) : QDialogButtonBox()
{
    for (const auto &info : list) {
        std::size_t index = 0;
        auto text = getIndexedContainerValue(info.texts(), index, DEFAULT_BUTTON_TEXT);
        auto button = addButton(text, DEFAULT_BUTTON_ROLE);

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
        std::shared_ptr<Button> button = nullptr;

        auto texts = info.texts();
        auto size = texts.size();

        if (size > 1) {
            stateButton_ = std::make_shared<StateButton>(texts);
            button = stateButton_;
        } else if (size == 1) {
            button = std::make_shared<Button>(texts.at(0));
            buttons_.push_back(button);
        } else {
            continue;
        }

        addWidget(button.get());

        connect(button.get(), &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

void ButtonPanelWidget::updateButtonState()
{
    stateButton_->updateState();
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
        { { EDIT_BUTTON_TEXT }, [this]() { updateCompanionAction(); } },
        { { CONNECT_BUTTON_TEXT, DISCONNECT_BUTTON_TEXT }, [=, this]() { group->clientAction(); } }
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
    buttons_->updateButtonState();

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

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object)
{
    logTemplateInfo(
        "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}",
        object->name(), object->ipAddress(), object->serverPort(), object->clientPort());
}
