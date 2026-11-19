#ifndef BODY_HPP
#define BODY_HPP

#include <memory>

#include <QLabel>
#include <QPushButton>

#include "widget_layout.hpp"

class MessageBodyWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageBodyWidget();
    ~MessageBodyWidget() = default;

    virtual void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) {}
    virtual void setParentTo(MessageWidget *widget) {}
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

protected:
    void buildText(MessagePtr message);
    virtual std::string getText(MessagePtr message) { return ""s; }

    std::shared_ptr<QLabel> label_;
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

#endif // BODY_HPP
