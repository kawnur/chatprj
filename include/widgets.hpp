#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <cstdint>
#include <memory>
#include <mutex>

#include <QColor>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

class CentralPanelWidget;
class Companion;
class LeftPanelWidget;
class Message;
class MessageWidget;
class RightPanelWidget;

QString getInitialConnectButtonLabel();
QString getNextConnectButtonLabel(QString &currentLabel);

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
    void send(const QString &);
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

    void initializeFields();  // non-virtual because is called from constructor
};

class SocketInfoWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoWidget() = default;
    SocketInfoWidget(const SocketInfoWidget &object);
    SocketInfoWidget(SocketInfoWidget &&) = default;

    SocketInfoWidget(
        std::string &name, std::string &ipAddress, uint16_t &serverPort, uint16_t &clientPort);

    SocketInfoWidget(
        std::string &&name, std::string &&ipAddress, uint16_t &&serverPort, uint16_t &&clientPort);

    SocketInfoWidget(std::shared_ptr<Companion> companion);
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
    void setNewMessagesIndicatorOff() override;

public slots:
    void requestHistoryFromCompanionAction();
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();
    void clientAction();

private:
    void initializeFields();
    void changeColor(QColor &color);
    void mousePressEvent(QMouseEvent *event) override;

    bool isSelected_;
    bool isConnected_;
    std::shared_ptr<Companion> companion_;
    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;
    QColor selectedColor_;
    QColor unselectedColor_;
    std::unique_ptr<QPalette> palette_;
    std::unique_ptr<QHBoxLayout> layout_;
    std::unique_ptr<IndicatorWidget> connectionStateIndicator_;
    std::unique_ptr<QLabel> nameLabel_;
    std::unique_ptr<QLabel> ipAddressLabel_;
    std::unique_ptr<QLabel> serverPortLabel_;
    std::unique_ptr<QLabel> clientPortLabel_;
    std::unique_ptr<QPushButton> editButton_;
    std::unique_ptr<QPushButton> connectButton_;
    std::unique_ptr<IndicatorWidget> newMessagesIndicator_;
    std::shared_ptr<QAction> requestHistoryAction_;

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
    WidgetGroup(std::shared_ptr<Companion> companion);
    ~WidgetGroup();

    void set();
    void addMessageWidgetToCentralPanelChatHistory(std::shared_ptr<Message> message);
    void clearChatHistory();
    void hideCentralPanel();
    void showCentralPanel();
    std::shared_ptr<SocketInfoBaseWidget> getSocketInfoBase();
    void sortChatHistoryElements();
    void messageAdded();
    void askUserForHistorySendingConfirmation();

signals:
    void addMessageWidgetToCentralPanelChatHistorySignal(std::shared_ptr<Message> message);
    void askUserForHistorySendingConfirmationSignal();
    void buildChatHistorySignal();    

public slots:
    void messageWidgetSelected(std::shared_ptr<MessageWidget> widget);
    void buildChatHistorySlot();

private slots:
    void addMessageWidgetToCentralPanelChatHistorySlot(std::shared_ptr<Message> message);
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

    void setParents(
        std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer);

    void hideSocketInfoStubWidget();
    void hideCentralPanel();
    void showCentralPanel();
    void hideStubPanels();
    void showStubPanels();
    void setLeftPanelWidth(int width);

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
    MainWindowContainerWidget(std::shared_ptr<QWidget> widget);
    ~MainWindowContainerWidget() = default;

    void addWidgetToLayout(std::shared_ptr<QWidget> widget);
    void addWidgetToLayoutAndSetParentTo(std::shared_ptr<QWidget> widget);

private:
    std::unique_ptr<QVBoxLayout> layout_;
};

#endif // WIDGETS_HPP
