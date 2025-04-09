#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <chrono>
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
#include <thread>

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
    QPalette* palettePtr_;

    void keyPressEvent(QKeyEvent*);

signals:
    void send(const QString&);
};

class IndicatorWidget : public QWidget {

    Q_OBJECT

public:
    IndicatorWidget(uint8_t, bool);
    IndicatorWidget(const IndicatorWidget*);
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
    QPalette* palettePtr_;
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
    SocketInfoWidget(Companion*);
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
    Companion* companionPtr_;
    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;
    QColor selectedColor_;
    QColor unselectedColor_;
    QPalette* palettePtr_;
    QHBoxLayout* layoutPtr_;
    IndicatorWidget* connectionStateIndicatorPtr_;
    QLabel* nameLabelPtr_;
    QLabel* ipAddressLabelPtr_;
    QLabel* serverPortLabelPtr_;
    QLabel* clientPortLabelPtr_;
    QPushButton* editButtonPtr_;
    QPushButton* connectButtonPtr_;
    IndicatorWidget* newMessagesIndicatorPtr_;
    QAction* requestHistoryAction_;

    void initializeFields();
    void changeColor(QColor&);
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

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
    QHBoxLayout* layoutPtr_;
    QLabel* markLabelPtr_;

    void initializeFields();
};

class ShowHideWidget : public QWidget {

    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget();

private:
    bool show_;
    QVBoxLayout* layoutPtr_;
    QLabel* labelPtr_;
    QPalette* palettePtr_;

    void hideInfo();
    void showInfo();
    void mousePressEvent(QMouseEvent*) override;
};

// class ScrollArea : public QScrollArea
// {
// public:
//     ScrollArea() = default;
//     ~ScrollArea() = default;

// private:

//     // void wheelEvent(QWheelEvent*) override;
// };

class WidgetGroup : public QObject {

    Q_OBJECT

public:
    WidgetGroup(Companion*);
    ~WidgetGroup();

    void set();
    void addMessageWidgetToCentralPanelChatHistory(const Message*, const MessageState*);
    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    SocketInfoBaseWidget* getSocketInfoBasePtr();
    void sortChatHistoryElements();
    void messageAdded();
    void askUserForHistorySendingConfirmation();

signals:
    void addMessageWidgetToCentralPanelChatHistorySignal(
        const MessageState*, const Message*);

    void askUserForHistorySendingConfirmationSignal();
    void buildChatHistorySignal();    

public slots:
    void messageWidgetSelected(MessageWidget*);
    void buildChatHistorySlot();

private slots:
    void addMessageWidgetToCentralPanelChatHistorySlot(
        const MessageState*, const Message*);

    void askUserForHistorySendingConfirmationSlot();

private:
    Companion* companionPtr_;
    SocketInfoBaseWidget* socketInfoBasePtr_;
    CentralPanelWidget* centralPanelPtr_;
    uint32_t antecedentMessagesCounter_;
    std::mutex antecedentMessagesCounterMutex_;
};

class StubWidgetGroup {
public:
    StubWidgetGroup();
    ~StubWidgetGroup();

    void set();
    void setParents(QWidget*, QWidget*);
    void hideSocketInfoStubWidget();
    void hideCentralPanel();
    void showCentralPanel();
    void hideStubPanels();
    void showStubPanels();
    void setLeftPanelWidth(int);

private:
    SocketInfoStubWidget* socketInfoPtr_;
    LeftPanelWidget* leftPanelPtr_;
    CentralPanelWidget* centralPanelPtr_;
    RightPanelWidget* rightPanelPtr_;
};

class MainWindowContainerWidget : public QWidget {

    Q_OBJECT

public:
    MainWindowContainerWidget(QWidget*);
    ~MainWindowContainerWidget();

    void addWidgetToLayout(QWidget*);
    void addWidgetToLayoutAndSetParentTo(QWidget*);

private:
    QVBoxLayout* layoutPtr_;
};

#endif // WIDGETS_HPP
