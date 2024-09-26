#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QVBoxLayout>

#include "application.hpp"
//#include "blocking_tcp_echo_client.hpp"
#include "db_interaction.hpp"
//#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"
#include "widgets.hpp"


class Companion;
class Message;
class SocketInfo;
class SocketInfoBaseWidget;
class SocketInfoWidget;
class TextEditWidget;


class WidgetGroup : public QObject
{
    Q_OBJECT

public:
    WidgetGroup() {};
    WidgetGroup(const Companion*);
    ~WidgetGroup() = default;

    SocketInfoBaseWidget* socketInfoBasePtr_;
    QPlainTextEdit* chatHistoryPtr_;
    QPalette* chatHistoryPalettePtr_;
    TextEditWidget* textEditPtr_;

    QString formatMessage(const Companion*, const Message*);
    void addMessageToChatHistory(const QString&);

public slots:
    void sendMessage();

private:
    QString buildChatHistory(const Companion*);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
//    static MainWindow* _instance;
//    std::queue<QString> messages_;
//    EchoClient* client_;

    // TODO rename panel child widgets according its functionality

    QWidget* centralWidgetPtr_;
    QHBoxLayout* centralWidgetLayoutPtr_;

    // left panel

    QWidget* leftPanelPtr_;
    QVBoxLayout* leftPanelLayoutPtr_;
//    QLineEdit* test_;

    // central panel

    QWidget* centralPanelPtr_;
    QVBoxLayout* centralPanelLayoutPtr_;
//    QGraphicsScene* graphicsScene_;
//    QGraphicsView* graphicsView_;
//    QRectF* rect_;
//    QGraphicsSimpleTextItem* textItem_;
//    int linesCount_;
//    QFont* font_;

    QLabel* companionNameLabelPtr_;
    QPalette* companionNameLabelPalettePtr_;
    QPlainTextEdit* chatHistoryWidgetStubPtr_;
    QPalette* chatHistoryWidgetStubPalettePtr_;

    TextEditWidget* textEditStubPtr_;
//    QPushButton* button_;

    // right panel

    QWidget* rightPanelPtr_;
    QVBoxLayout* rightPanelLayoutPtr_;
    QPlainTextEdit* appLogWidgetPtr_;
    QPalette* appLogWidgetPalettePtr_;

//    QPushButton* testPlainTextEditButton_;

    std::map<const Companion*, WidgetGroup*> map_;

    void addStubWidgetToLeftPanel();
//    void addSocketInfoWidgetToLeftPanel(SocketInfo*);
    void addSocketInfoWidgetToLeftPanel(const Companion*);
    void addChatHistoryWidgetToMapping(const Companion*);
    void addTextEditWidgetToMapping(const Companion*);

//    void testMainWindowRightPanel();
    void addTestSocketInfoWidgetToLeftPanel();

    SocketInfoBaseWidget* getMappedSocketInfoBaseWidgetByCompanion(const Companion*) const;

//    MainWindow(QWidget* parent = nullptr);
    void closeEvent(QCloseEvent*) override;
    virtual ~MainWindow();

    void keyPressEvent(QKeyEvent*) override;

public:
    MainWindow();
//    static MainWindow* instance();

    const Companion* selectedCompanion_;  // TODO make private

    const Companion* getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget*) const;
    const Companion* getMappedCompanionByTextEditWidget(TextEditWidget*) const;
    const Companion* getMappedCompanionByWidgetGroup(WidgetGroup*) const;


    void resetSelectedCompanion(const Companion*);

    void buildWidgetGroups(std::vector<Companion*>*);
    void set();
    void setLeftPanel();
    void addTextToChatHistoryWidget(const QString&);
    void addTextToAppLogWidget(const QString&);

    void addWidgetToLeftPanel(SocketInfoBaseWidget*);
    void addWidgetToCentralPanel(QWidget*);
};

MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
