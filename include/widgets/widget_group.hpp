#ifndef WIDGET_GROUP_HPP
#define WIDGET_GROUP_HPP

#include <memory>
#include <mutex>

#include "alias.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "widgets_panel.hpp"

class WidgetGroup : public QObject, public std::enable_shared_from_this<WidgetGroup>
{
    Q_OBJECT

public:
    WidgetGroup(CompanionPtr companion);
    ~WidgetGroup();

    void set();
    void clearHistory();
    void hideFields();
    void hideCentralPanel();
    void showCentralPanel();
    void messageAdded();
    void updateSocketInfoWidget();
    void select();
    void unselect();
    void setNewMessagesIndicatorOn();
    void setNewMessagesIndicatorOff();

signals:
    void addIncomingTextMessageWidgetSignal(IncomingTextMessagePtr message);
    void addOutgoingTextMessageWidgetSignal(OutgoingTextMessagePtr message);
    void addIncomingFileMessageWidgetSignal(IncomingFileMessagePtr message);
    void addOutgoingFileMessageWidgetSignal(OutgoingFileMessagePtr message);
    // void addTextMessageWidgetsSignal(const std::vector<TextMessagePtr> &messages);  // TODO rename
    // void askUserForHistorySendingConfirmationSignal();
    // void buildChatHistorySignal();

public slots:
    // void messageWidgetSelected(MessageWidgetPtr widget);
    void incomingTextMessageWidgetSelected(IncomingTextMessageWidgetPtr widget);
    void outgoingTextMessageWidgetSelected(OutgoingTextMessageWidgetPtr widget);
    // void buildChatHistorySlot();
    void requestHistorySlot();
    void clientAction();

private slots:
    void addIncomingTextMessageWidgetSlot(IncomingTextMessagePtr message);
    void addOutgoingTextMessageWidgetSlot(OutgoingTextMessagePtr message);
    void addIncomingFileMessageWidgetSlot(IncomingFileMessagePtr message);
    void addOutgoingFileMessageWidgetSlot(OutgoingFileMessagePtr message);
    // void addFileMessageWidgetSlot(MessagePtr message);
    // void addTextMessageWidgetsSlot(const std::vector<TextMessagePtr> &messages);
    // void askUserForHistorySendingConfirmationSlot();

private:
    void incrementCounter();

    template<class T>
    void addMessageWidget(TPtr<T> message)
    {
        // bool isAntecedent = message->isAntecedent();

        // if (isAntecedent) {
        //     std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

        //     ++antecedentMessagesCounter_;

        //     logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
        // }

        // auto widget = std::make_shared<T>(companion_, message);
        // auto result = getGraphicManager()->setMappedMessageWidget(companion_, message, widget);
        // widget->set(shared_from_this());
        // centralPanel_->addMessage(widget, isAntecedent);
        // messageAdded();

        // {
        //     std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

        //     ++antecedentMessagesCounter_;

        //     logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
        // }

        auto widget = std::make_shared<typename T::widget_type>(companion_, message);
        auto result = getGraphicManager()->setMappedMessageWidget(companion_, message, widget);
        widget->set(shared_from_this());
        // widget->set();
        centralPanel_->addMessage(widget);
        messageAdded();
    }

    CompanionPtr companion_;
    SocketInfoBaseWidgetPtr socketInfoBase_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;
    uint32_t counter_;  // counts new messages
    std::mutex counterMutex_;
};

class StubWidgetGroup
{
public:
    StubWidgetGroup();
    ~StubWidgetGroup() = default;

    void hide();
    void show();
    void hideSocketInfo();
    void hideCentral();
    void showCentral();
    void setLeftPanelWidth(int width);
    std::shared_ptr<SocketInfoBaseWidget> socketInfoBase() const;
    std::shared_ptr<LeftPanelWidget> left() const;
    std::shared_ptr<CentralPanelWidget> central() const;
    std::shared_ptr<RightPanelWidget> right() const;

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfo_;
    std::shared_ptr<LeftPanelWidget> left_;
    std::shared_ptr<CentralPanelWidget> central_;
    std::shared_ptr<RightPanelWidget> right_;
};

#endif // WIDGET_GROUP_HPP
