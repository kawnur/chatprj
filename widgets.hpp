#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "constants.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "logging.hpp"

class Companion;
class CompanionAction;
class Message;
class MainWindow;

const QString CONNECT_BUTTON_CONNECT_LABEL { "Connect" };  // TODO move to constants
const QString CONNECT_BUTTON_DISCONNECT_LABEL { "Disconnect" };

const std::vector<QString> connectButtonLabels
{
    CONNECT_BUTTON_CONNECT_LABEL,
    CONNECT_BUTTON_DISCONNECT_LABEL
};

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
    void send();
};

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    IndicatorWidget();
    IndicatorWidget(const IndicatorWidget*);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();
    void setMe();

public slots:
    void toggle();

private:
    bool isOn_;
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
    // SocketInfoWidget(QString&, QString&, QString&);
    // SocketInfoWidget(QString&&, QString&&, QString&&);
//    SocketInfo(const SocketInfo&) {};
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {};
    SocketInfoWidget(std::string&, std::string&, uint16_t&, uint16_t&);
    SocketInfoWidget(std::string&&, std::string&&, uint16_t&&, uint16_t&&);
    SocketInfoWidget(Companion*);
    ~SocketInfoWidget();

    void print();
    bool isStub() override;

    void select();
    void unselect();

    void update();

public slots:
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();
    void clientAction();

private:
    Companion* companionPtr_;

    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;

    QColor selectedColor_;
    QColor unselectedColor_;
    QPalette* palettePtr_;

    QHBoxLayout* layoutPtr_;

    IndicatorWidget* indicatorPtr_;
    QLabel* nameLabelPtr_;
    QLabel* ipAddressLabelPtr_;
    QLabel* serverPortLabelPtr_;
    QLabel* clientPortLabelPtr_;
    QPushButton* editButtonPtr_;
    QPushButton* connectButtonPtr_;
//    QPushButton* toggleIndicatorButton_;

    void initializeFields();

    void slotCustomMenuRequested(QPoint);

    void changeColor(QColor&);

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

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

    void mousePressEvent(QMouseEvent*) override;
    void hideInfo();
    void showInfo();
};

class LeftPanelWidget : public QWidget
{
    Q_OBJECT

public:
    LeftPanelWidget(QWidget*);
    ~LeftPanelWidget();

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    size_t getCompanionPanelChildrenSize();
    void removeStubsFromCompanionPanel();  // TODO do we need remove?
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);  // TODO do we nedd remove?

private:
    QVBoxLayout* layoutPtr_;
    QPalette* palettePtr_;

    QWidget* companionPanelPtr_;
    QVBoxLayout* companionPanelLayoutPtr_;

    QSpacerItem* spacerPtr_;
    // ShowHideWidget* showHideWidgetPtr_;
};

class CentralPanelWidget : public QWidget
{
    Q_OBJECT

public:
    CentralPanelWidget(QWidget*, const std::string&);
    ~CentralPanelWidget();

    void set(Companion*);

    void hideWidgets();
    void showWidgets();
    // void oldSelectedCompanionActions(const Companion*);
    // void newSelectedCompanionActions(const Companion*);

    void addMessageToChatHistory(const QString&);
    void clearChatHistory();

private:
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
    QPlainTextEdit* chatHistoryWidgetPtr_;
    QPalette* chatHistoryWidgetPalettePtr_;

    TextEditWidget* textEditPtr_;
    QPalette* textEditPalettePtr_;
    //    QPushButton* button_;

    void sendMessage();
};

class RightPanelWidget : public QWidget
{
    Q_OBJECT

public:
    RightPanelWidget(QWidget*);
    ~RightPanelWidget();

    void set();
    void addTextToAppLogWidget(const QString&);

private:
    QVBoxLayout* layoutPtr_;
    QPlainTextEdit* appLogWidgetPtr_;
    QPalette* appLogWidgetPalettePtr_;
};

class WidgetGroup : public QObject
{
    Q_OBJECT

public:
    WidgetGroup() = default;
    WidgetGroup(const Companion*);
    ~WidgetGroup();

    QString formatMessage(const Companion*, const Message*);  // TODO move to widget
    void addMessageToChatHistory(const QString&);
    void clearChatHistory();

    void hideCentralPanel();
    void showCentralPanel();

    SocketInfoBaseWidget* getSocketInfoBasePtr();

public slots:
    // void sendMessage();
    // void connectToServer();

private:
    SocketInfoBaseWidget* socketInfoBasePtr_;

    // QPlainTextEdit* chatHistoryPtr_;
    // QPalette* chatHistoryPalettePtr_;
    // TextEditWidget* textEditPtr_;
    // QPalette* textEditPalettePtr_;
    CentralPanelWidget* centralPanelPtr_;

    QString buildChatHistory(const Companion*);
};

class StubWidgetGroup
{
public:
    StubWidgetGroup();
    ~StubWidgetGroup();

    void set();
    void setParents(QWidget*, QWidget*);
    void hideCentralPanel();
    void showCentralPanel();

    void hideStubPanels();
    void showStubPanels();

    void setStubPanelsSize(const QSize&, const QSize&, const QSize&);

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

class CompanionDataDialog : public QDialog
{
    Q_OBJECT

public:
    CompanionDataDialog(CompanionActionType, QWidget*, Companion*);
    ~CompanionDataDialog();

    void set(CompanionAction*);    

    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();

private:
    CompanionActionType actionType_;
    CompanionAction* actionPtr_;

    QFormLayout* layoutPtr_;

    QLabel* nameLabelPtr_;
    QLineEdit* nameEditPtr_;

    QLabel* ipAddressLabelPtr_;
    QLineEdit* ipAddressEditPtr_;

    QLabel* portLabelPtr_;
    QLineEdit* portEditPtr_;

    QDialogButtonBox* buttonBoxPtr_;

    // void sendData();
};

class TextDialog : public QDialog
{
    Q_OBJECT

public:
    TextDialog(QDialog*, QWidget*, DialogType, const std::string&);
    ~TextDialog();

    void set(QDialog*);

private:
    CompanionActionType actionType_;

    QDialog* parentDialogPtr_;
    QPlainTextEdit* textEditPtr_;

    void closeDialogs();

protected:
    QVBoxLayout* layoutPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class TwoButtonsTextDialog : public TextDialog
{
    Q_OBJECT

public:
    TwoButtonsTextDialog(QDialog*, QWidget*, DialogType, const std::string&, std::string&&);
    ~TwoButtonsTextDialog() = default;

    void set(CompanionAction*);

private:
    CompanionAction* actionPtr_;
    std::string buttonText_;

    void acceptAction();
};

#endif // WIDGETS_HPP
