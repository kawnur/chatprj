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

class MessageIndicatorPanelWidget : public QWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget(MessagePtr message);
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
    Q_OBJECT

public:
    MessageBodyWidget() = default;
    ~MessageBodyWidget() = default;

    virtual void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) {}
    virtual void setParentTo(MessageWidget *widget) {}
    virtual void set() {}

protected:
    WidgetType type_;  // TODO change enum
};

class TextMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    TextMessageBodyWidget(MessagePtr message);
    ~TextMessageBodyWidget() = default;

    void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) override;
    void setParentTo(MessageWidget *widget) override;

private:
    std::shared_ptr<QLabel> label_;
};

class FileMessageBodyWidget : public MessageBodyWidget
{
    Q_OBJECT

public:
    FileMessageBodyWidget(MessagePtr message);
    ~FileMessageBodyWidget() = default;

    void addToParentLayout(std::shared_ptr<QVBoxLayout> layout) override;
    void setParentTo(MessageWidget *widget) override;
    void set() override;

private slots:
    void saveFileSlot();

private:
    void buildText(MessagePtr message);

    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QPushButton> button_;  // download button
};

// class MessageWidget : public QWidget, public std::enable_shared_from_this<MessageWidget>
class MessageWidget : public Widget, public std::enable_shared_from_this<MessageWidget>
{
    Q_OBJECT

public:
    MessageWidget(CompanionPtr companion, MessagePtr message, QWidgetPtr parent = nullptr);
    virtual ~MessageWidget() {}

    MessagePtr message();
    void set(WidgetGroupPtr group);

    void saveFile();
    void setSent();
    void setReceived();

signals:
    void widgetSelectedSignal(MessageWidgetPtr widget);

protected:
    virtual void addMembersToLayout() {}

    MessageWidgetType type_;
    CompanionPtr companion_;
    MessagePtr message_;
    // std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> header_;
    MessageBodyWidgetPtr body_;
    std::shared_ptr<MessageIndicatorPanelWidget> indicatorPanel_;

private:
    void initBody();
    void mousePressEvent(QMouseEvent *) override;
};

QString formatHeader(CompanionPtr companion, MessagePtr message);
QString formatBody(MessagePtr message);

#endif // WIDGETS_MESSAGE_HPP
