#include "widget_group.hpp"

#include "companion.hpp"
#include "widgets_message.hpp"

const uint16_t CENTRAL_STUB_FIXED_WIDTH = 400;

WidgetGroup::WidgetGroup(CompanionPtr companion)
    : companion_(companion), counterMutex_(), counter_(0) {}

WidgetGroup::~WidgetGroup()
{
    logArgsInfo(Q_FUNC_INFO);
    getGraphicManager()->removeCompanionWidget(socketInfoBase_);
    hideCentralPanel();
}

void WidgetGroup::set()
{
    auto graphicManager = getGraphicManager();

    // socket
    auto widget = std::make_shared<SocketInfoWidget>(companion_);
    widget->set(shared_from_this());

    socketInfoBase_ = dynamic_pointer_cast<SocketInfoBaseWidget>(widget);
    graphicManager->addCompanionWidget(socketInfoBase_);

    // central panel
    centralPanel_ = std::make_shared<CentralPanelWidget>(companion_->name(), false);

    centralPanel_->set(companion_);
    graphicManager->addWidgetToCentralContainer(centralPanel_);
    centralPanel_->hide();

    // connections
    connect(
        this, &WidgetGroup::addIncomingTextMessageWidgetSignal,
        this, &WidgetGroup::addIncomingTextMessageWidgetSlot, Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::addOutgoingTextMessageWidgetSignal,
        this, &WidgetGroup::addOutgoingTextMessageWidgetSlot, Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::addIncomingFileMessageWidgetSignal,
        this, &WidgetGroup::addIncomingFileMessageWidgetSlot, Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::addOutgoingFileMessageWidgetSignal,
        this, &WidgetGroup::addOutgoingFileMessageWidgetSlot, Qt::QueuedConnection);
}

void WidgetGroup::clearHistory()
{
    centralPanel_->clearHistory();
    setNewMessagesIndicatorOff();
    // socketInfoBase_->updateNewMessagesIndicator();

    std::lock_guard<std::mutex> lock(counterMutex_);
    counter_ = 0;
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
    std::lock_guard<std::mutex> lock(counterMutex_);

    // set new message indicator on if socket info widget is not selected
    if (counter_ <= 0)
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

void WidgetGroup::incomingTextMessageWidgetSelected(IncomingTextMessageWidgetPtr widget)
{
    std::lock_guard<std::mutex> lock(counterMutex_);

    auto message = widget->message();
    auto messageState = message->state();

    if (!messageState)
        return;

    bool isRead = messageState->isRead();

    if (isRead)
        return;

    if (counter_ != 0)  // ???
        --counter_;

    logTemplateInfo("counter_: {}", counter_);

    auto cast = std::dynamic_pointer_cast<IncomingTextMessage>(message);

    if (cast)
        getGraphicManager()->markMessageAsRead(cast);

    if (counter_ == 0)
        setNewMessagesIndicatorOff();
    // socketInfoBase_->updateNewMessagesIndicator();
}

void WidgetGroup::outgoingTextMessageWidgetSelected(OutgoingTextMessageWidgetPtr widget) {}

// void WidgetGroup::buildChatHistorySlot()
// {
//     companion_->addMessageWidgetsToChatHistory();
// }

void WidgetGroup::incrementCounter()
{
    std::lock_guard<std::mutex> lock(counterMutex_);

    ++counter_;

    logTemplateInfo("antecedentMessagesCounter_: {}", counter_);
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

void WidgetGroup::addIncomingTextMessageWidgetSlot(IncomingTextMessagePtr message)
{
    if (!message->isRead())
        incrementCounter();

    addMessageWidget<IncomingTextMessage>(message);
}

void WidgetGroup::addOutgoingTextMessageWidgetSlot(OutgoingTextMessagePtr message)
{
    addMessageWidget<OutgoingTextMessage>(message);
}

void WidgetGroup::addIncomingFileMessageWidgetSlot(IncomingFileMessagePtr message)
{
    addMessageWidget<IncomingFileMessage>(message);
}

void WidgetGroup::addOutgoingFileMessageWidgetSlot(OutgoingFileMessagePtr message)
{
    addMessageWidget<OutgoingFileMessage>(message);
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
    central_->setFixedWidth(CENTRAL_STUB_FIXED_WIDTH);
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
