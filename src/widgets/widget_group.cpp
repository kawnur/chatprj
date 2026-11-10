#include "widget_group.hpp"

#include "companion.hpp"
#include "message_state.hpp"
#include "widgets_message.hpp"

const uint16_t CENTRAL_STUB_FIXED_WIDTH = 400;

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
    graphicManager->addWidgetToCentralContainer(centralPanel_);
    centralPanel_->hide();

    connect(
        this, &WidgetGroup::addTextMessageWidgetSignal,
        this, &WidgetGroup::addTextMessageWidgetSlot, Qt::QueuedConnection);

    connect(
        this, &WidgetGroup::addFileMessageWidgetSignal,
        this, &WidgetGroup::addFileMessageWidgetSlot, Qt::QueuedConnection);

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

// void WidgetGroup::addMessageWidgetSlot(MessagePtr message)
// {
//     bool isAntecedent = message->isAntecedent();

//     if (isAntecedent) {
//         std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

//         ++antecedentMessagesCounter_;

//         logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
//     }

//     // auto widget = getGraphicManager()->buildMessageWidget(companion_, message);
//     auto widget = std::make_shared<MessageWidget>(companion_, message);

//     // auto lambda = [=]()
//     // {
//     //     // companion->setMappedMessageWidget(message, widget);
//     //     getManager()->setMappedMessageWidget(companion_, message, widget);
//     // };

//     // runInDetachedThread(lambda);
//     auto result = getGraphicManager()->setMappedMessageWidget(companion_, message, widget);
//     // widget->setBase(shared_from_this());
//     widget->set(shared_from_this());
//     centralPanel_->addMessage(widget, isAntecedent);
//     messageAdded();
// }

void WidgetGroup::addMessageWidgetsSlot(const std::vector<MessagePtr> &messages)
{
    for (auto message : messages)
        addTextMessageWidgetSlot(message);

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

void WidgetGroup::addTextMessageWidgetSlot(MessagePtr message)
{
    addMessageWidget<TextMessageWidget>(message);
}

void WidgetGroup::addFileMessageWidgetSlot(MessagePtr message)
{
    addMessageWidget<FileMessageWidget>(message);
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
