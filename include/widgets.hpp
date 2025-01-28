#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <chrono>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <thread>

#include "constants.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "logging.hpp"
#include "utils.hpp"

class Action;
class Companion;
class CompanionAction;
class MainWindow;
class Message;
class MessageState;

QString getInitialConnectButtonLabel();
QString getNextConnectButtonLabel();

class TextEditWidget : public QTextEdit
{
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

class IndicatorWidget : public QWidget
{
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

class SocketInfoStubWidget : public SocketInfoBaseWidget
{
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

class ShowHideWidget : public QWidget
{
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

class MessageIndicatorPanelWidget : public QWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget(bool, const MessageState*);
    ~MessageIndicatorPanelWidget();

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();
    void unsetNewMessageLabel();

private:
    bool isMessageFromMe_;
    QHBoxLayout* layoutPtr_;
    IndicatorWidget* sentIndicatoPtr_;
    IndicatorWidget* receivedIndicatoPtr_;
    QLabel* newMessageLabelPtr_;
};

class WidgetGroup;

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    MessageWidget(QWidget*, const Companion*, const MessageState*, const Message*);
    ~MessageWidget();

    void set(const WidgetGroup*);
    void setMessageWidgetAsSent();
    void setMessageWidgetAsReceived();

signals:
    void widgetSelectedSignal(MessageWidget*);

private:
    // bool createdAsAntecedent_;
    bool isMessageFromMe_;
    QPalette* palettePtr_;
    QVBoxLayout* layoutPtr_;
    QLabel* headerLabelPtr_;
    QLabel* messageLabelPtr_;
    MessageIndicatorPanelWidget* indicatorPanelPtr_;

    void mousePressEvent(QMouseEvent*) override;
};

class LeftPanelWidget : public QWidget
{
    Q_OBJECT

public:
    LeftPanelWidget(QWidget*);
    ~LeftPanelWidget();

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);
    int getLastCompanionPanelChildWidth();

private:
    QVBoxLayout* layoutPtr_;
    QPalette* palettePtr_;
    QWidget* companionPanelPtr_;
    QVBoxLayout* companionPanelLayoutPtr_;
    QSpacerItem* spacerPtr_;
};

class CentralPanelWidget : public QWidget
{
    Q_OBJECT

public:
    CentralPanelWidget(QWidget*, const std::string&);
    ~CentralPanelWidget();

    void set(Companion*);

    void addMessageWidgetToChatHistory(
        const WidgetGroup*, const Companion*, const Message*, const MessageState*);

    // TODO delete 'FromThread' methods, emit signal by caller
    void addMessageWidgetToChatHistoryFromThread(
        const WidgetGroup*, const Companion*, const Message*, const MessageState*);

    void scrollDownChatHistory();
    void clearChatHistory();
    void sortChatHistoryElements(bool);

private slots:
    void addMessageWidgetToChatHistorySlot(
        const WidgetGroup*, const Companion*, const Message*, const MessageState*);

signals:
    void addMessageWidgetToChatHistorySignal(
        const WidgetGroup*, const Companion*, const Message*, const MessageState*);

private:
    std::mutex chatHistoryMutex_;
    int scrollBarMax = 0;
    Companion* companionPtr_;
    QVBoxLayout* layoutPtr_;

    //    QGraphicsScene* graphicsScene_;
    //    QGraphicsView* graphicsView_;
    //    QRectF* rect_;
    //    QGraphicsSimpleTextItem* textItem_;
    //    int linesCount_;
    //    QFont* font_;

    QLabel* companionNameLabelPtr_;
    QPalette* companionNameLabelPalettePtr_;
    QPalette* chatHistoryWidgetPalettePtr_;
    QScrollArea* chatHistoryScrollAreaPtr_;
    QWidget* chatHistoryWidgetPtr_;
    QVBoxLayout* chatHistoryLayoutPtr_;
    TextEditWidget* textEditPtr_;
    QPalette* textEditPalettePtr_;

    void sendMessage(const QString&);
};

class RightPanelWidget : public QWidget
{
    Q_OBJECT

public:
    RightPanelWidget(QWidget*);
    ~RightPanelWidget();

    void set();
    void addTextToAppLogWidget(const QString&);

public slots:
    void clearLogAction();

signals:
    void addTextToAppLogWidgetSignal(const QString&);

private:
    QVBoxLayout* layoutPtr_;
    QPlainTextEdit* appLogWidgetPtr_;
    QPalette* appLogWidgetPalettePtr_;

private slots:
    void addTextToAppLogWidgetSlot(const QString&);
    void customMenuRequestedSlot(QPoint);
};

class WidgetGroup : public QObject
{
    Q_OBJECT

public:
    WidgetGroup(const Companion*);
    ~WidgetGroup();

    void set();
    void buildChatHistory();

    void addMessageWidgetToCentralPanelChatHistory(
        const WidgetGroup*, const Message*, const MessageState*);

    // TODO delete 'FromThread' methods, emit signal by caller
    void addMessageWidgetToCentralPanelChatHistoryFromThread(
        const MessageState*, const Message*);

    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    SocketInfoBaseWidget* getSocketInfoBasePtr();
    void sortChatHistoryElements();
    void messageAdded();
    void askUserForHistorySendingConfirmation();

    // TODO delete 'FromThread' methods, emit signal by caller
    void askUserForHistorySendingConfirmationFromThread();

signals:
    void askUserForHistorySendingConfirmationSignal();
    void buildChatHistorySignal();

public slots:
    void messageWidgetSelected(MessageWidget*);
    void buildChatHistorySlot();

private slots:
    void askUserForHistorySendingConfirmationSlot();

private:
    const Companion* companionPtr_;
    SocketInfoBaseWidget* socketInfoBasePtr_;
    CentralPanelWidget* centralPanelPtr_;
    uint32_t antecedentMessagesCounter_;
    std::mutex antecedentMessagesCounterMutex_;
};

class StubWidgetGroup
{
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

class MainWindowContainerWidget : public QWidget
{
    Q_OBJECT

public:
    MainWindowContainerWidget(QWidget*);
    ~MainWindowContainerWidget();

    void addWidgetToLayout(QWidget*);
    void addWidgetToLayoutAndSetParentTo(QWidget*);

private:
    QVBoxLayout* layoutPtr_;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog() = default;
    ~Dialog() = default;

    void setAction(Action* actionPtr) { this->actionPtr_ = actionPtr; }
    virtual void set() {}

protected:
    Action* actionPtr_;
};

class CompanionDataDialog : public Dialog
{
    Q_OBJECT

public:
    CompanionDataDialog(CompanionActionType, QWidget*, Companion*);
    ~CompanionDataDialog();

    void set() override;
    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();

private:
    CompanionActionType actionType_;
    QFormLayout* layoutPtr_;
    QLabel* nameLabelPtr_;
    QLineEdit* nameEditPtr_;
    QLabel* ipAddressLabelPtr_;
    QLineEdit* ipAddressEditPtr_;
    QLabel* portLabelPtr_;
    QLineEdit* portEditPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class CreatePasswordDialog : public Dialog
{
    Q_OBJECT

public:
    CreatePasswordDialog();
    ~CreatePasswordDialog();

    void set() override;
    std::string getFirstEditText();
    std::string getSecondEditText();

private:
    QFormLayout* layoutPtr_;
    QLabel* firstLabelPtr_;
    QLineEdit* firstEditPtr_;
    QLabel* secondLabelPtr_;
    QLineEdit* secondEditPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class GetPasswordDialog : public Dialog
{
    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog();

    void set() override;
    std::string getEditText();

private:
    QFormLayout* layoutPtr_;
    QLabel* labelPtr_;
    QLineEdit* editPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class TextDialog;

class ButtonInfo
{
public:
    ButtonInfo(
        const QString&, QDialogButtonBox::ButtonRole, void (TextDialog::*)());

    ~ButtonInfo() = default;

    QString buttonText_;
    QDialogButtonBox::ButtonRole buttonRole_;
    void (TextDialog::*functionPtr_)();
};

class TextDialog : public Dialog
{
    Q_OBJECT

public:
    TextDialog(QWidget*, DialogType, const QString&, std::vector<ButtonInfo>*);
    ~TextDialog();

    void set() override;
    void closeSelf();
    void closeSelfAndParentDialog();
    void acceptAction();

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    QPlainTextEdit* textEditPtr_;
    QVBoxLayout* layoutPtr_;
    QDialogButtonBox* buttonBoxPtr_;
    std::vector<ButtonInfo>* buttonsInfoPtr_;
};

#endif // WIDGETS_HPP
