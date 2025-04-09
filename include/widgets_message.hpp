#ifndef WIDGETS_MESSAGE_HPP
#define WIDGETS_MESSAGE_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "constants.hpp"
#include "logging.hpp"
#include "utils.hpp"
#include "widgets.hpp"

class Companion;
class IndicatorWidget;
class Message;
class MessageState;
class WidgetGroup;

class MessageIndicatorPanelWidget : public QWidget {

    Q_OBJECT

public:
    MessageIndicatorPanelWidget(bool, const MessageState*);
    ~MessageIndicatorPanelWidget();

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();
    void unsetNewMessageLabel();

private:
    bool isMessageFromMe_;
    QHBoxLayout* layoutPtr_;
    IndicatorWidget* sentIndicatoPtr_;
    IndicatorWidget* receivedIndicatoPtr_;
    QLabel* newMessageLabelPtr_;
};

class MessageWidget : public QWidget {

    Q_OBJECT

public:
    MessageWidget(QWidget*, Companion*, const MessageState*, const Message*);
    virtual ~MessageWidget();

    void setBase(const WidgetGroup*);
    void setMessageWidgetAsSent();
    void setMessageWidgetAsReceived();

    virtual void set(const WidgetGroup*) {}

signals:
    void widgetSelectedSignal(MessageWidget*);

protected:
    Companion* companionPtr_;
    const Message* messagePtr_;
    bool isMessageFromMe_;
    QPalette* palettePtr_;
    QVBoxLayout* layoutPtr_;
    QLabel* headerLabelPtr_;
    QLabel* messageLabelPtr_;
    MessageIndicatorPanelWidget* indicatorPanelPtr_;

    virtual void addMembersToLayout() {}

private:
    void mousePressEvent(QMouseEvent*) override;
};

class TextMessageWidget : public MessageWidget {

    Q_OBJECT

public:
    TextMessageWidget(QWidget*, Companion*, const MessageState*, const Message*);
    ~TextMessageWidget();

private:
    void addMembersToLayout() override;
};

class FileMessageWidget : public MessageWidget {

    Q_OBJECT

public:
    FileMessageWidget(QWidget*, Companion*, const MessageState*, const Message*);

    ~FileMessageWidget();

    void set(const WidgetGroup*) override;

private:
    bool showButton_;
    const MessageState* messageStatePtr_;
    QWidget* fileWidgetPtr_;
    QHBoxLayout* fileWidgetLayoutPtr_;
    QPushButton* downloadButtonPtr_;

    void addMembersToLayout() override;

private slots:
    // void sendFileRequest(bool);
    void saveFileSlot();
};

#endif // WIDGETS_MESSAGE_HPP
