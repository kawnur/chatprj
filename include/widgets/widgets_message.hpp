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

class MessageBodyWidget : public QWidget
{
public:
    MessageBodyWidget() = default;
    ~MessageBodyWidget() = default;
};

class TextMessageBodyWidget : public MessageBodyWidget
{
public:
    TextMessageBodyWidget(MessagePtr message);
    ~TextMessageBodyWidget() = default;

private:
    std::shared_ptr<QLabel> label_;
};

class FileMessageBodyWidget : public MessageBodyWidget
{
public:
    FileMessageBodyWidget(MessagePtr message);
    ~FileMessageBodyWidget() = default;

private:
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QPushButton> button_;  // download button
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

    WidgetType type_;
    // bool fromMe_;
    CompanionPtr companion_;
    MessagePtr message_;
    std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> header_;
    // std::shared_ptr<QLabel> messageLabel_;
    MessageBodyWidgetPtr body_;
    std::shared_ptr<MessageIndicatorPanelWidget> indicatorPanel_;

private:
    MessageBodyWidgetPtr initBody(MessagePtr message);
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

// class FileWidget : public QWidget
// {
// public:
//     FileWidget();
//     ~FileWidget();


// private:
//     std::shared_ptr<QHBoxLayout> layout_;
// };

class FileMessageWidget : public MessageWidget
{
    Q_OBJECT

public:
    FileMessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    ~FileMessageWidget() = default;

    void set(WidgetGroupPtr) override;

private:
    void buildText(MessagePtr message);

    QWidgetPtr fileWidget_;
    // std::shared_ptr<QHBoxLayout> fileWidgetLayout_;
    // std::unique_ptr<FileWidget> fileWidget_;
    std::shared_ptr<QPushButton> downloadButton_;

    void addMembersToLayout() override;

private slots:
    void saveFileSlot();
};

// std::pair<std::string, std::string> formatMessageHeaderAndBody(
//     CompanionPtr companion, MessagePtr message);

QString formatHeader(CompanionPtr companion, MessagePtr message);
QString formatBody(CompanionPtr companion, MessagePtr message);

#endif // WIDGETS_MESSAGE_HPP
