#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <chrono>
#include <memory>
#include <thread>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
// #include <QFormLayout>
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

class TextEditWidget : public QTextEdit {

    Q_OBJECT

public:
    TextEditWidget();
    ~TextEditWidget();

private:
    std::shared_ptr<QPalette> palettePtr_;

    void keyPressEvent(std::shared_ptr<QKeyEvent>);

signals:
    void send(const QString&);
};

class IndicatorWidget : public QWidget {

    Q_OBJECT

public:
    IndicatorWidget(uint8_t, bool);
    IndicatorWidget(std::shared_ptr<IndicatorWidget>);
    ~IndicatorWidget();

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
    std::shared_ptr<QPalette> palettePtr_;
};

class SocketInfoBaseWidget : public QWidget {

    Q_OBJECT

public:
    SocketInfoBaseWidget() = default;
    SocketInfoBaseWidget(const SocketInfoBaseWidget&) = default;
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    void initializeFields();  // non-virtual because is called from constructor
};

class SocketInfoWidget : public SocketInfoBaseWidget {

    Q_OBJECT

public:
    SocketInfoWidget();
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {}
    SocketInfoWidget(std::string&, std::string&, uint16_t&, uint16_t&);
    SocketInfoWidget(std::string&&, std::string&&, uint16_t&&, uint16_t&&);
    SocketInfoWidget(std::shared_ptr<Companion>);
    ~SocketInfoWidget();

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
    std::shared_ptr<Companion> companionPtr_;
    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;
    QColor selectedColor_;
    QColor unselectedColor_;
    std::shared_ptr<QPalette> palettePtr_;
    std::shared_ptr<QHBoxLayout> layoutPtr_;
    std::shared_ptr<IndicatorWidget> connectionStateIndicatorPtr_;
    std::shared_ptr<QLabel> nameLabelPtr_;
    std::shared_ptr<QLabel> ipAddressLabelPtr_;
    std::shared_ptr<QLabel> serverPortLabelPtr_;
    std::shared_ptr<QLabel> clientPortLabelPtr_;
    std::shared_ptr<QPushButton> editButtonPtr_;
    std::shared_ptr<QPushButton> connectButtonPtr_;
    std::shared_ptr<IndicatorWidget> newMessagesIndicatorPtr_;
    std::shared_ptr<QAction> requestHistoryAction_;

    void initializeFields();
    void changeColor(QColor&);
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private slots:
    void customMenuRequestedSlot(QPoint);
};

class SocketInfoStubWidget : public SocketInfoBaseWidget {

    Q_OBJECT

public:
    SocketInfoStubWidget();
    ~SocketInfoStubWidget() = default;

    bool isStub() override;

private:
    QString mark_;
    std::shared_ptr<QHBoxLayout> layoutPtr_;
    std::shared_ptr<QLabel> markLabelPtr_;

    void initializeFields();
};

class ShowHideWidget : public QWidget {

    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget();

private:
    bool show_;
    std::shared_ptr<QVBoxLayout> layoutPtr_;
    std::shared_ptr<QLabel> labelPtr_;
    std::shared_ptr<QPalette> palettePtr_;

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

class WidgetGroup : public QObject {

    Q_OBJECT

public:
    WidgetGroup(std::shared_ptr<Companion>);
    ~WidgetGroup();

    void set();
    void addMessageWidgetToCentralPanelChatHistory(std::shared_ptr<Message>, std::shared_ptr<MessageState>);
    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    std::shared_ptr<SocketInfoBaseWidget> getSocketInfoBasePtr();
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
    std::shared_ptr<Companion> companionPtr_;
    std::shared_ptr<SocketInfoBaseWidget> socketInfoBasePtr_;
    std::shared_ptr<CentralPanelWidget> centralPanelPtr_;
    uint32_t antecedentMessagesCounter_;
    std::mutex antecedentMessagesCounterMutex_;
};

class StubWidgetGroup {
public:
    StubWidgetGroup();
    ~StubWidgetGroup();

    void set();
    void setParents(std::shared_ptr<QWidget>, std::shared_ptr<QWidget>);
    void hideSocketInfoStubWidget();
    void hideCentralPanel();
    void showCentralPanel();
    void hideStubPanels();
    void showStubPanels();
    void setLeftPanelWidth(int);

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfoPtr_;
    std::shared_ptr<LeftPanelWidget> leftPanelPtr_;
    std::shared_ptr<CentralPanelWidget> centralPanelPtr_;
    std::shared_ptr<RightPanelWidget> rightPanelPtr_;
};

class MainWindowContainerWidget : public QWidget {

    Q_OBJECT

public:
    MainWindowContainerWidget(std::shared_ptr<QWidget>);
    ~MainWindowContainerWidget();

    void addWidgetToLayout(std::shared_ptr<QWidget>);
    void addWidgetToLayoutAndSetParentTo(std::shared_ptr<QWidget>);

private:
    std::shared_ptr<QVBoxLayout> layoutPtr_;
};

#endif // WIDGETS_HPP
