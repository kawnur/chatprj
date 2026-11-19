#ifndef WIDGETS_MESSAGE_HPP
#define WIDGETS_MESSAGE_HPP

#include <memory>

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"
#include "message_widget_body.hpp"
#include "message_widget_header.hpp"
#include "message_widget_indicator_panel.hpp"

class MessageWidget : public VLayoutWidget
{
    Q_OBJECT

public:
    MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~MessageWidget() {}

    MessagePtr message();
    virtual void set(WidgetGroupPtr group) {}

    virtual void setSent() {}
    virtual void setReceived() {}

protected:
    CompanionPtr companion_;
    MessagePtr message_;
};

class TextMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~TextMessageWidget() = default;
};

class IncomingTextMessageWidget
    : public TextMessageWidget, public std::enable_shared_from_this<IncomingTextMessageWidget>
{
    Q_OBJECT

public:
    IncomingTextMessageWidget(
        CompanionPtr companion, IncomingTextMessagePtr message, QWidgetPtr parent = nullptr);

    ~IncomingTextMessageWidget() = default;

    void set(WidgetGroupPtr group) override;

signals:
    void widgetSelectedSignal(IncomingTextMessageWidgetPtr widget);

private:
    void mousePressEvent(QMouseEvent *) override;

    std::shared_ptr<IncomingMessageHeaderWidget> header_;
    std::shared_ptr<IncomingTextMessageBodyWidget> body_;
    std::shared_ptr<IncomingTextMessageIndicatorPanelWidget> indicatorPanel_;
};

class OutgoingTextMessageWidget
    : public TextMessageWidget, public std::enable_shared_from_this<OutgoingTextMessageWidget>
{
    Q_OBJECT

public:
    OutgoingTextMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~OutgoingTextMessageWidget() = default;

    void set(WidgetGroupPtr group) override;
    void setSent() override;
    void setReceived() override;

signals:
    void widgetSelectedSignal(OutgoingTextMessageWidgetPtr widget);

private:
    void mousePressEvent(QMouseEvent *) override;

    std::shared_ptr<OutgoingMessageHeaderWidget> header_;
    std::shared_ptr<OutgoingTextMessageBodyWidget> body_;
    std::shared_ptr<OutgoingTextMessageIndicatorPanelWidget> indicatorPanel_;
};

class FileMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~FileMessageWidget() = default;
};

class IncomingFileMessageWidget : public FileMessageWidget
{
    Q_OBJECT

public:
    IncomingFileMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~IncomingFileMessageWidget() = default;

    void set(WidgetGroupPtr group) override;

private slots:
    void saveFileSlot();

private:
    std::shared_ptr<IncomingMessageHeaderWidget> header_;
    std::shared_ptr<IncomingFileMessageBodyWidget> body_;
};

class OutgoingFileMessageWidget : public FileMessageWidget
{
    Q_OBJECT

public:
    OutgoingFileMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~OutgoingFileMessageWidget() = default;

private:
    std::shared_ptr<OutgoingMessageHeaderWidget> header_;
    std::shared_ptr<OutgoingFileMessageBodyWidget> body_;
};

#endif // WIDGETS_MESSAGE_HPP
