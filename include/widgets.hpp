#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <chrono>
#include <memory>
#include <thread>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>

#include "constants.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "logging.hpp"
#include "utils.hpp"
#include "widgets_message.hpp"
#include "widgets_panel.hpp"

class Action;
class CentralPanelWidget;
class Companion;
class CompanionAction;
class FileAction;
class LeftPanelWidget;
class MainWindow;
class Message;
class MessageState;
class RightPanelWidget;

QString getInitialConnectButtonLabel();
QString getNextConnectButtonLabel();

class TextEditWidget : public QTextEdit
{
    Q_OBJECT

public:
    TextEditWidget();
    ~TextEditWidget() = default;

private:
    std::shared_ptr<QPalette> palette_;

    void keyPressEvent(std::shared_ptr<QKeyEvent>);

signals:
    void send(const QString&);
};

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    IndicatorWidget(uint8_t, bool);
    IndicatorWidget(std::shared_ptr<IndicatorWidget>);
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
    SocketInfoBaseWidget(const SocketInfoBaseWidget&) = default;
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    void initializeFields();  // non-virtual because is called from constructor
};

class SocketInfoWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoWidget();
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {}
    SocketInfoWidget(std::string&, std::string&, uint16_t&, uint16_t&);
    SocketInfoWidget(std::string&&, std::string&&, uint16_t&&, uint16_t&&);
    SocketInfoWidget(std::shared_ptr<Companion>);
    ~SocketInfoWidget() = default;

    QString getName() const;
    QString getIpAddress() const;
    uint16_t getServerPort() const;
    uint16_t getClientPort() const;

    bool isStub() override;
    bool isSelected();
    void select();
    void unselect();
    void update();
    void setNewMessagesIndicatorOn();
    void setNewMessagesIndicatorOff();

public slots:
    void requestHistoryFromCompanionAction();
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();
    void clientAction();

private:
    bool isSelected_;
    bool isConnected_;
    std::shared_ptr<Companion> companion_;
    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;
    QColor selectedColor_;
    QColor unselectedColor_;
    std::shared_ptr<QPalette> palette_;
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<IndicatorWidget> connectionStateIndicator_;
    std::shared_ptr<QLabel> nameLabel_;
    std::shared_ptr<QLabel> ipAddressLabel_;
    std::shared_ptr<QLabel> serverPortLabel_;
    std::shared_ptr<QLabel> clientPortLabel_;
    std::shared_ptr<QPushButton> editButton_;
    std::shared_ptr<QPushButton> connectButton_;
    std::shared_ptr<IndicatorWidget> newMessagesIndicator_;
    std::shared_ptr<QAction> requestHistoryAction_;

    void initializeFields();
    void changeColor(QColor&);
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private slots:
    void customMenuRequestedSlot(QPoint);
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
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<QLabel> markLabel_;

    void initializeFields();
};

class ShowHideWidget : public QWidget
{
    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget() = default;

private:
    bool show_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QPalette> palette_;

    void hideInfo();
    void showInfo();
    void mousePressEvent(QMouseEvent *) override;
};

// class ScrollArea : public QScrollArea
// {
// public:
//     ScrollArea() = default;
//     ~ScrollArea() = default;

// private:

//     // void wheelEvent(std::shared_ptr<QWheelEvent>) override;
// };

class WidgetGroup : public QObject
{
    Q_OBJECT

public:
    WidgetGroup(std::shared_ptr<Companion>);
    ~WidgetGroup();

    void set();
    void addMessageWidgetToCentralPanelChatHistory(std::shared_ptr<Message>, std::shared_ptr<MessageState>);
    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    std::shared_ptr<SocketInfoBaseWidget> getSocketInfoBase();
    void sortChatHistoryElements();
    void messageAdded();
    void askUserForHistorySendingConfirmation();

signals:
    void addMessageWidgetToCentralPanelChatHistorySignal(
        std::shared_ptr<MessageState>, std::shared_ptr<Message>);

    void askUserForHistorySendingConfirmationSignal();
    void buildChatHistorySignal();    

public slots:
    void messageWidgetSelected(std::shared_ptr<MessageWidget>);
    void buildChatHistorySlot();

private slots:
    void addMessageWidgetToCentralPanelChatHistorySlot(
        std::shared_ptr<MessageState>, std::shared_ptr<Message>);

    void askUserForHistorySendingConfirmationSlot();

private:
    std::shared_ptr<Companion> companion_;
    std::shared_ptr<SocketInfoBaseWidget> socketInfoBase_;
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
    void setParents(std::shared_ptr<QWidget>, std::shared_ptr<QWidget>);
    void hideSocketInfoStubWidget();
    void hideCentralPanel();
    void showCentralPanel();
    void hideStubPanels();
    void showStubPanels();
    void setLeftPanelWidth(int);

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfo_;
    std::shared_ptr<LeftPanelWidget> leftPanel_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;
    std::shared_ptr<RightPanelWidget> rightPanel_;
};

class MainWindowContainerWidget : public QWidget
{
    Q_OBJECT

public:
    MainWindowContainerWidget(std::shared_ptr<QWidget>);
    ~MainWindowContainerWidget() = default;

    void addWidgetToLayout(std::shared_ptr<QWidget>);
    void addWidgetToLayoutAndSetParentTo(std::shared_ptr<QWidget>);

private:
    std::shared_ptr<QVBoxLayout> layout_;
};

#endif // WIDGETS_HPP
