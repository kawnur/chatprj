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
#include "widgets.hpp"

class MessageIndicatorPanelWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget();
    virtual ~MessageIndicatorPanelWidget() = default;
};

class IncomingTextMessageIndicatorPanelWidget : public MessageIndicatorPanelWidget
{
    Q_OBJECT

public:
    IncomingTextMessageIndicatorPanelWidget(MessagePtr message);
    ~IncomingTextMessageIndicatorPanelWidget() = default;

    void unsetNewMessageLabel();

private:
    std::shared_ptr<QLabel> newMessageLabel_;
};

class OutgoingTextMessageIndicatorPanelWidget : public MessageIndicatorPanelWidget
{
    Q_OBJECT

public:
    OutgoingTextMessageIndicatorPanelWidget(MessagePtr message);
    ~OutgoingTextMessageIndicatorPanelWidget() = default;

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();

private:
    IndicatorWidgetPtr sent_;
    IndicatorWidgetPtr received_;
};

class MessageBodyWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageBodyWidget();
    ~MessageBodyWidget() = default;

    virtual void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) {}
    virtual void setParentTo(MessageWidget *widget) {}
    virtual void set() {}
};

class TextMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    TextMessageBodyWidget(MessagePtr message);
    ~TextMessageBodyWidget() = default;

private:
    std::shared_ptr<QLabel> label_;
};

class FileMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    FileMessageBodyWidget(MessagePtr message);
    ~FileMessageBodyWidget() = default;

    void set() override;

private slots:
    void saveFileSlot();

private:
    void buildText(MessagePtr message);

    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QPushButton> button_;  // download button
};

class MessageWidget : public VLayoutWidget, public std::enable_shared_from_this<MessageWidget>
{
    Q_OBJECT

public:
    MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~MessageWidget() {}

    MessagePtr message();
    void set(WidgetGroupPtr group);

    void saveFile();
    virtual void setSent() {}
    virtual void setReceived() {}

signals:
    void widgetSelectedSignal(MessageWidgetPtr widget);

protected:
    void addMembersToLayout(MessageBodyWidgetPtr body);

    CompanionPtr companion_;
    MessagePtr message_;
    std::shared_ptr<QLabel> header_;
};

class TextMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~TextMessageWidget() = default;

private:
    std::shared_ptr<TextMessageBodyWidget> body_;
};

class IncomingTextMessageWidget : public TextMessageWidget
{
public:
    IncomingTextMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~IncomingTextMessageWidget() = default;

private:
    void mousePressEvent(QMouseEvent *) override;

    std::shared_ptr<IncomingTextMessageIndicatorPanelWidget> indicatorPanel_;
};

class OutgoingTextMessageWidget : public TextMessageWidget
{
public:
    OutgoingTextMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~OutgoingTextMessageWidget() = default;

    void setSent() override;
    void setReceived() override;

private:
    void mousePressEvent(QMouseEvent *) override;

    std::shared_ptr<OutgoingTextMessageIndicatorPanelWidget> indicatorPanel_;
};

class FileMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    ~FileMessageWidget() = default;

private:
    std::shared_ptr<FileMessageBodyWidget> body_;
};

QString formatHeader(CompanionPtr companion, MessagePtr message);
QString formatBody(MessagePtr message);

#endif // WIDGETS_MESSAGE_HPP
