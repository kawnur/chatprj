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

class MessageHeaderWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    // MessageHeaderWidget(/*CompanionPtr companion, MessagePtr message*/);
    MessageHeaderWidget();

    virtual ~MessageHeaderWidget() = default;

protected:
    void setLabel(
        const std::string &color, const std::string &sender, const std::string &receiver,
        const std::string &time);

    std::shared_ptr<QLabel> label_;
};

class IncomingMessageHeaderWidget : public MessageHeaderWidget
{
    Q_OBJECT

public:
    IncomingMessageHeaderWidget(CompanionPtr companion, MessagePtr message);
    ~IncomingMessageHeaderWidget() = default;
};

class OutgoingMessageHeaderWidget : public MessageHeaderWidget
{
    Q_OBJECT

public:
    OutgoingMessageHeaderWidget(CompanionPtr companion, MessagePtr message);
    ~OutgoingMessageHeaderWidget() = default;
};

class MessageBodyWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageBodyWidget();
    ~MessageBodyWidget() = default;

    virtual void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) {}
    virtual void setParentTo(MessageWidget *widget) {}
    // virtual void set() {}
};

class TextMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    TextMessageBodyWidget(WidgetType type, MessagePtr message);
    virtual ~TextMessageBodyWidget() = default;

private:
    std::shared_ptr<QLabel> label_;
};

class IncomingTextMessageBodyWidget : public TextMessageBodyWidget
{
    Q_OBJECT

public:
    IncomingTextMessageBodyWidget(MessagePtr message);
    ~IncomingTextMessageBodyWidget() = default;
};

class OutgoingTextMessageBodyWidget : public TextMessageBodyWidget
{
    Q_OBJECT

public:
    OutgoingTextMessageBodyWidget(MessagePtr message);
    ~OutgoingTextMessageBodyWidget() = default;
};

class FileMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    FileMessageBodyWidget(WidgetType type, MessagePtr message);
    virtual ~FileMessageBodyWidget() = default;

    // void set() override;

// std::shared_ptr<QPushButton> button() const;

// private slots:
    // void saveFileSlot();

protected:
    void buildText(MessagePtr message);
    virtual std::string getText(MessagePtr message) { return ""s; }

    std::shared_ptr<QLabel> label_;
    // std::shared_ptr<QPushButton> button_;  // download button
};

class IncomingFileMessageBodyWidget : public FileMessageBodyWidget
{
    Q_OBJECT

public:
    IncomingFileMessageBodyWidget(MessagePtr message);
    ~IncomingFileMessageBodyWidget() = default;

    std::shared_ptr<QPushButton> button() const;

private:
    std::string getText(MessagePtr message) override;

    std::shared_ptr<QPushButton> button_;  // download button
};

class OutgoingFileMessageBodyWidget : public FileMessageBodyWidget
{
    Q_OBJECT

public:
    OutgoingFileMessageBodyWidget(MessagePtr message);
    ~OutgoingFileMessageBodyWidget() = default;

private:
    std::string getText(MessagePtr message) override;
};

class MessageWidget : public VLayoutWidget/*, public std::enable_shared_from_this<MessageWidget>*/
{
    Q_OBJECT

public:
    MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~MessageWidget() {}

    MessagePtr message();
    // virtual MessagePtr getMessage() { return nullptr; }
    // void set(WidgetGroupPtr group);
    virtual void set(WidgetGroupPtr group) {}

    // void saveFile();
    virtual void setSent() {}
    virtual void setReceived() {}

// signals:
//     void widgetSelectedSignal(MessageWidgetPtr widget);

protected:
    // void addMembersToLayout(MessageBodyWidgetPtr body);

    CompanionPtr companion_;
    MessagePtr message_;
    // std::shared_ptr<QLabel> header_;
};

class TextMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    TextMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~TextMessageWidget() = default;

// private:
    // std::shared_ptr<TextMessageBodyWidget> body_;
};

class IncomingTextMessageWidget
    : public TextMessageWidget, public std::enable_shared_from_this<IncomingTextMessageWidget>
{
    Q_OBJECT

public:
    IncomingTextMessageWidget(
        CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);

    ~IncomingTextMessageWidget() = default;

    void set(WidgetGroupPtr group) override;

    // MessagePtr getMessage() override { return std::dynamic_pointer_cast<Message>(message_); }

    // IncomingTextMessagePtr message() const;

signals:
    void widgetSelectedSignal(IncomingTextMessageWidgetPtr widget);

private:
    void mousePressEvent(QMouseEvent *) override;

    // IncomingTextMessagePtr message_;

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

    // MessagePtr getMessage() override { return std::dynamic_pointer_cast<Message>(message_); }

    // OutgoingTextMessagePtr message() const;

signals:
    void widgetSelectedSignal(OutgoingTextMessageWidgetPtr widget);

private:
    void mousePressEvent(QMouseEvent *) override;

    // OutgoingTextMessagePtr message_;

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

    // MessagePtr getMessage() override { return message_; }

    // MessagePtr message() const;

private:
//     std::shared_ptr<FileMessageBodyWidget> body_;
protected:
    // MessagePtr message_;
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

// QString formatHeader(CompanionPtr companion, MessagePtr message);
// QString formatBody(MessagePtr message);

#endif // WIDGETS_MESSAGE_HPP
