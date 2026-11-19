#include "widget_socket_info.hpp"

#include "companion.hpp"
#include "data.hpp"
#include "widget_group.hpp"

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
    std::vector<ButtonInfo> list = {
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
    std::vector<MenuItemInfo> info {
        { { "Clear chat history" }, [this]() { clearHistoryAction(); } },
        { { "Delete companion" }, [this]() { deleteCompanionAction(); } },
        {
            { "Request chat history from companion" },
            [&]() { group->requestHistorySlot(); },
            true,
            true
        }
    };

    menu_ = std::make_unique<Menu>(info);

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
    menu_->updateSwitchableItems();
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
    menu_->popup(mapToGlobal(position));
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

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object)
{
    logTemplateInfo(
        "name: {0}, ipAddress: {1}, serverPort: {2}, clientPort: {3}",
        object->name(), object->ipAddress(), object->serverPort(), object->clientPort());
}
