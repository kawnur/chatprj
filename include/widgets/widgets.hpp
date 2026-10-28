#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <cstdint>
#include <memory>
#include <mutex>

#include <QColor>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTextEdit>

#include "alias.hpp"
#include "logging.hpp"

ButtonType getInitialConnectButtonType();
ButtonType getNextConnectButtonType(ButtonType current);

// class Button : public QPushButton
// {
//     Q_OBJECT

// public:
//     Button(ButtonType type);
//     ~Button();

// private:
//     ButtonType type_;
// };

class ButtonInfo
{
public:
    ButtonInfo();
    ButtonInfo(ButtonType type, std::function<void()> function);
    ~ButtonInfo();

    ButtonType type() const;
    std::function<void()> function() const;

private:
    ButtonType type_;
    std::function<void()> function_;
};

class ButtonBox :public QDialogButtonBox
{
    Q_OBJECT

public:
    ButtonBox(const std::initializer_list<ButtonInfo> &list);
    ~ButtonBox();

private:
    // std::vector<Button> buttons;

};

class TextEditWidget : public QTextEdit
{
    Q_OBJECT

public:
    TextEditWidget();
    ~TextEditWidget() = default;

private:
    std::unique_ptr<QPalette> palette_;

    void keyPressEvent(QKeyEvent *event);

signals:
    void sendSignal(const QString &text);

public slots:
    void sendSlot();
};

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    IndicatorWidget(uint8_t size, bool isOn);
    IndicatorWidget(std::shared_ptr<IndicatorWidget> indicator);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();
    void setMe();

public slots:
    void toggle();

private:
    bool isOn_;
    uint8_t size_;
    QColor onColor_;
    QColor offColor_;
    QColor meColor_;
    std::shared_ptr<QPalette> palette_;
};

class SocketInfoBaseWidget : public QWidget
{
    Q_OBJECT

public:
    SocketInfoBaseWidget() = default;
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    virtual void select() {}
    virtual void unselect() {}
    virtual void setNewMessagesIndicatorOn() {}
    virtual void setNewMessagesIndicatorOff() {}
    virtual void updateFields() {}
    virtual bool isConnected() { return false; }
    virtual void updateState() {}
    virtual CompanionPtr companion() { return nullptr; }

    void initializeFields();  // non-virtual because is called from constructor
};

class Menu : public QMenu
{
public:
    Menu() = default;
    ~Menu() { logArgsInfo(Q_FUNC_INFO); }
};

class SocketInfoWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoWidget(WidgetGroupPtr group, CompanionPtr companion);
    ~SocketInfoWidget() = default;

    QString name() const;
    QString ipAddress() const;
    QString serverPort() const;
    QString clientPort() const;

    bool isStub() override;
    bool isSelected();
    bool isConnected() override;
    void select() override;
    void unselect() override;
    void updateFields() override;
    void setNewMessagesIndicatorOn() override;
    void setNewMessagesIndicatorOff() override;

    void updateState() override;

    CompanionPtr companion() override;

public slots:
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();

private:
    void buildFields(WidgetGroupPtr group);
    void setFieldsFromCompanionData();
    void setFields();
    void changeColor(QColor &color);
    void mousePressEvent(QMouseEvent *event) override;

    bool isSelected_;
    bool isConnected_;
    CompanionPtr companion_;
    QColor selected_;
    QColor unselected_;
    std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<IndicatorWidget> state_;
    std::shared_ptr<QLabel> name_;
    std::shared_ptr<QLabel> ipAddress_;
    std::shared_ptr<QLabel> serverPort_;
    std::shared_ptr<QLabel> clientPort_;
    std::shared_ptr<QPushButton> edit_;
    ButtonType connectButtonType_;
    std::shared_ptr<QPushButton> connect_;
    std::shared_ptr<IndicatorWidget> received_;
    std::shared_ptr<QAction> requestHistory_;

private slots:
    void customMenuRequestedSlot(QPoint position);
};

class SocketInfoStubWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoStubWidget();
    ~SocketInfoStubWidget() = default;

    bool isStub() override;

private:
    QString mark_;
    std::unique_ptr<QHBoxLayout> layout_;
    std::unique_ptr<QLabel> markLabel_;

    void initializeFields();
};

class ShowHideWidget : public QWidget
{
    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget() = default;

private:
    void hideInfo();
    void showInfo();
    void mousePressEvent(QMouseEvent *event) override;

    bool show_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QPalette> palette_;
};

// class ScrollArea : public QScrollArea
// {
// public:
//     ScrollArea() = default;
//     ~ScrollArea() = default;

// private:

//     // void wheelEvent(std::shared_ptr<QWheelEvent>) override;
// };

class WidgetGroup : public QObject, public std::enable_shared_from_this<WidgetGroup>
{
    Q_OBJECT

public:
    WidgetGroup(CompanionPtr companion);
    ~WidgetGroup();

    void set();
    void clearHistory();
    void hideCentralPanel();
    void showCentralPanel();
    void messageAdded();
    void updateSocketInfoWidget();
    void select();
    void unselect();
    void setNewMessagesIndicatorOn();
    void setNewMessagesIndicatorOff();

signals:
    void addMessageWidgetSignal(MessagePtr message);
    void addMessageWidgetsSignal(const std::vector<MessagePtr> &messages);
    // void askUserForHistorySendingConfirmationSignal();
    // void buildChatHistorySignal();

public slots:
    void messageWidgetSelected(MessageWidgetPtr widget);
    // void buildChatHistorySlot();
    void requestHistorySlot();
    void clientAction();

private slots:
    void addMessageWidgetSlot(MessagePtr message);
    void addMessageWidgetsSlot(const std::vector<MessagePtr> &messages);
    // void askUserForHistorySendingConfirmationSlot();

private:
    CompanionPtr companion_;
    SocketInfoBaseWidgetPtr socketInfoBase_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;
    uint32_t antecedentMessagesCounter_;
    std::mutex antecedentMessagesCounterMutex_;
};

class StubWidgetGroup
{
public:
    StubWidgetGroup();
    ~StubWidgetGroup() = default;

    void hide();
    void show();
    void setParents(QWidgetPtr leftContainer, QWidgetPtr centralContainer);
    void hideSocketInfo();
    void hideCentral();
    void showCentral();
    void setLeftPanelWidth(int width);
    std::shared_ptr<SocketInfoBaseWidget> socketInfoBase() const;
    std::shared_ptr<LeftPanelWidget> left() const;
    std::shared_ptr<CentralPanelWidget> central() const;
    std::shared_ptr<RightPanelWidget> right() const;

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfo_;
    std::shared_ptr<LeftPanelWidget> left_;
    std::shared_ptr<CentralPanelWidget> central_;
    std::shared_ptr<RightPanelWidget> right_;
};

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object);

#endif // WIDGETS_HPP
