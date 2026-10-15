#ifndef WIDGETS_MESSAGE_HPP
#define WIDGETS_MESSAGE_HPP

#include <memory>

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "alias.hpp"

class MessageIndicatorPanelWidget : public QWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget(bool sentByMe, MessagePtr message);
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
    MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~MessageWidget() {}

    MessagePtr message();
    virtual void set(WidgetGroupPtr) {}
    void setBase(WidgetGroupPtr group);
    void setSent();
    void setReceived();
    std::shared_ptr<QLabel> messageLabel();

signals:
    void widgetSelectedSignal(MessageWidgetPtr widget);

protected:
    virtual void addMembersToLayout() {}

    bool isMessageFromMe_;
    CompanionPtr companion_;
    MessagePtr message_;
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
    TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    ~TextMessageWidget();

private:
    void addMembersToLayout() override;
};

class FileMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    ~FileMessageWidget() = default;

    void set(WidgetGroupPtr) override;

private:
    bool showButton_;
    QWidgetPtr fileWidget_;
    std::shared_ptr<QHBoxLayout> fileWidgetLayout_;
    std::shared_ptr<QPushButton> downloadButton_;

    void addMembersToLayout() override;

private slots:
    void saveFileSlot();
};

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    CompanionPtr companion, MessagePtr message);

#endif // WIDGETS_MESSAGE_HPP
