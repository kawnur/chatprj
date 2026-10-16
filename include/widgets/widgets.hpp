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
    SocketInfoBaseWidget(const SocketInfoBaseWidget &) = default;
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    virtual void setNewMessagesIndicatorOff() {}
    virtual void updateFields() {}

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
    SocketInfoWidget(CompanionPtr companion);
    ~SocketInfoWidget() = default;

    QString name() const;
    QString ipAddress() const;
    QString serverPort() const;
    QString clientPort() const;

    bool isStub() override;
    bool isSelected();
    void select();
    void unselect();
    void updateFields() override;
    void setNewMessagesIndicatorOn();
    void setNewMessagesIndicatorOff() override;

public slots:
    void requestHistoryFromCompanionAction();
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();
    void clientAction();

private:
    void buildFields();
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
    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    SocketInfoBaseWidgetPtr getSocketInfoBase();
    void sortChatHistoryElements();
    void messageAdded();
    void askUserForHistorySendingConfirmation();
    void updateSocketInfoWidget();

signals:
    void addMessageWidgetSignal(MessagePtr message);
    void askUserForHistorySendingConfirmationSignal();
    // void buildChatHistorySignal();

public slots:
    void messageWidgetSelected(MessageWidgetPtr widget);
    // void buildChatHistorySlot();

private slots:
    void addMessageWidgetSlot(MessagePtr message);
    void askUserForHistorySendingConfirmationSlot();

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

    void set();

    void setParents(
        QWidgetPtr leftContainer, QWidgetPtr centralContainer);

    void hideSocketInfoStubWidget();
    void hideCentralPanel();
    void showCentralPanel();
    void hideStubPanels();
    void showStubPanels();
    void setLeftPanelWidth(int width);

    std::shared_ptr<SocketInfoBaseWidget> socketInfoBase() const;
    std::shared_ptr<LeftPanelWidget> leftPanel() const;
    std::shared_ptr<CentralPanelWidget> centralPanel() const;
    std::shared_ptr<RightPanelWidget> rightPanel() const;

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfo_;
    std::shared_ptr<LeftPanelWidget> leftPanel_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;
    std::shared_ptr<RightPanelWidget> rightPanel_;
};

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object);

#endif // WIDGETS_HPP
