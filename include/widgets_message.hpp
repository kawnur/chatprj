#ifndef WIDGETS_MESSAGE_HPP
#define WIDGETS_MESSAGE_HPP

#include <memory>

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class Companion;
class IndicatorWidget;
class Message;
class WidgetGroup;

class MessageIndicatorPanelWidget : public QWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget(bool sentByMe, std::shared_ptr<Message> message);
    ~MessageIndicatorPanelWidget() = default;

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();
    void unsetNewMessageLabel();

private:
    bool sentByMe_;
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<IndicatorWidget> sent_;
    std::shared_ptr<IndicatorWidget> received_;
    std::shared_ptr<QLabel> newMessageLabel_;
};

class MessageWidget : public QWidget, public std::enable_shared_from_this<MessageWidget>
{
    Q_OBJECT

public:
    MessageWidget(
        std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
        std::shared_ptr<Message> message);

    virtual ~MessageWidget() {}

    virtual void set(std::shared_ptr<WidgetGroup>) {}

    void setBase(std::shared_ptr<WidgetGroup> group);
    void setMessageWidgetAsSent();
    void setMessageWidgetAsReceived();

signals:
    void widgetSelectedSignal(std::shared_ptr<MessageWidget> widget);

protected:
    virtual void addMembersToLayout() {}

    bool isMessageFromMe_;
    std::shared_ptr<Companion> companion_;
    std::shared_ptr<Message> message_;
    std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> headerLabel_;
    std::shared_ptr<QLabel> messageLabel_;
    std::shared_ptr<MessageIndicatorPanelWidget> indicatorPanel_;

private:
    void mousePressEvent(QMouseEvent *) override;
};

class TextMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    TextMessageWidget(
        std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
        std::shared_ptr<Message> message);

    ~TextMessageWidget();

private:
    void addMembersToLayout() override;
};

class FileMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    FileMessageWidget(
        std::shared_ptr<QWidget> parent, std::shared_ptr<Companion> companion,
        std::shared_ptr<Message> message);

    ~FileMessageWidget() = default;

    void set(std::shared_ptr<WidgetGroup>) override;

private:
    bool showButton_;
    std::shared_ptr<QWidget> fileWidget_;
    std::shared_ptr<QHBoxLayout> fileWidgetLayout_;
    std::shared_ptr<QPushButton> downloadButton_;

    void addMembersToLayout() override;

private slots:
    void saveFileSlot();
};

#endif // WIDGETS_MESSAGE_HPP
