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
    void addTextMessageWidgetSignal(MessagePtr message);
    void addFileMessageWidgetSignal(MessagePtr message);
    void addMessageWidgetsSignal(const std::vector<MessagePtr> &messages);  // TODO rename
    // void askUserForHistorySendingConfirmationSignal();
    // void buildChatHistorySignal();

public slots:
    void messageWidgetSelected(MessageWidgetPtr widget);
    // void buildChatHistorySlot();
    void requestHistorySlot();
    void clientAction();

private slots:
    void addTextMessageWidgetSlot(MessagePtr message);
    void addFileMessageWidgetSlot(MessagePtr message);
    void addMessageWidgetsSlot(const std::vector<MessagePtr> &messages);
    // void askUserForHistorySendingConfirmationSlot();

private:
    template<typename T>
    void addMessageWidget(MessagePtr message)
    {
        bool isAntecedent = message->isAntecedent();

        if (isAntecedent) {
            std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

            ++antecedentMessagesCounter_;

            logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
        }

        // auto widget = getGraphicManager()->buildMessageWidget(companion_, message);
        auto widget = std::make_shared<T>(companion_, message);

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

    CompanionPtr companion_;
    SocketInfoBaseWidgetPtr socketInfoBase_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;
    uint32_t antecedentMessagesCounter_;
    std::mutex antecedentMessagesCounterMutex_;
};

#endif // WIDGET_GROUP_HPP
