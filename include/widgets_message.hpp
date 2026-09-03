#ifndef WIDGETS_MESSAGE_HPP
#define WIDGETS_MESSAGE_HPP

#include <memory>

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
    MessageIndicatorPanelWidget(bool, std::shared_ptr<MessageState>);
    ~MessageIndicatorPanelWidget() = default;

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();
    void unsetNewMessageLabel();

private:
    bool isMessageFromMe_;
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<IndicatorWidget> sentIndicato_;
    std::shared_ptr<IndicatorWidget> receivedIndicato_;
    std::shared_ptr<QLabel> newMessageLabel_;
};

class MessageWidget : public QWidget {

    Q_OBJECT

public:
    MessageWidget(std::shared_ptr<QWidget>, std::shared_ptr<Companion>, std::shared_ptr<MessageState>, std::shared_ptr<Message>);
    virtual ~MessageWidget();

    void setBase(std::shared_ptr<WidgetGroup>);
    void setMessageWidgetAsSent();
    void setMessageWidgetAsReceived();

    virtual void set(std::shared_ptr<WidgetGroup>) {}

signals:
    void widgetSelectedSignal(std::shared_ptr<MessageWidget>);

protected:
    std::shared_ptr<Companion> companion_;
    std::shared_ptr<Message> message_;
    bool isMessageFromMe_;
    std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> headerLabel_;
    std::shared_ptr<QLabel> messageLabel_;
    std::shared_ptr<MessageIndicatorPanelWidget> indicatorPanel_;

    virtual void addMembersToLayout() {}

private:
    void mousePressEvent(QMouseEvent *) override;
};

class TextMessageWidget : public MessageWidget {

    Q_OBJECT

public:
    TextMessageWidget(std::shared_ptr<QWidget>, std::shared_ptr<Companion>, std::shared_ptr<MessageState>, std::shared_ptr<Message>);
    ~TextMessageWidget();

private:
    void addMembersToLayout() override;
};

class FileMessageWidget : public MessageWidget {

    Q_OBJECT

public:
    FileMessageWidget(
        std::shared_ptr<QWidget>, std::shared_ptr<Companion>, std::shared_ptr<MessageState>,
        std::shared_ptr<Message>);

    ~FileMessageWidget() = default;

    void set(std::shared_ptr<WidgetGroup>) override;

private:
    bool showButton_;
    std::shared_ptr<MessageState> messageState_;
    std::shared_ptr<QWidget> fileWidget_;
    std::shared_ptr<QHBoxLayout> fileWidgetLayout_;
    std::shared_ptr<QPushButton> downloadButton_;

    void addMembersToLayout() override;

private slots:
    // void sendFileRequest(bool);
    void saveFileSlot();
};

#endif // WIDGETS_MESSAGE_HPP
