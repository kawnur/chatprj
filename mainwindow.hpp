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
class SocketInfo;
class SocketInfoBaseWidget;
class SocketInfoWidget;
class TextEditWidget;

struct ChatHistoryGroup {
    QPlainTextEdit* textEdit_;
    QPalette* palette_;
};

struct WidgetGroup {
    SocketInfoBaseWidget* socketInfoBase_;
    TextEditWidget* textEdit_;
    ChatHistoryGroup* chatHistory_;
};

class MainWindow : public QMainWindow {

    Q_OBJECT

private:
//    static MainWindow* _instance;
//    std::queue<QString> messages_;
//    EchoClient* client_;

    // TODO rename panel child widgets according its functionality

    QWidget* centralWidget_;
    QHBoxLayout* centralWidgetLayout_;

    QWidget* leftPanel_;
    QVBoxLayout* leftPanelLayout_;
//    QLineEdit* test_;

    QWidget* centralPanel_;
    QVBoxLayout* centralPanelLayout_;
//    QGraphicsScene* graphicsScene_;
//    QGraphicsView* graphicsView_;
//    QRectF* rect_;
//    QGraphicsSimpleTextItem* textItem_;
//    int linesCount_;
//    QFont* font_;

    QLabel* companionNameLabel_;
    QPalette* companionNameLabelPalette_;
    QPlainTextEdit* chatHistoryWidgetStub_;
    QPalette* chatHistoryWidgetStubPalette_;

    TextEditWidget* textEditStub_;
//    QPushButton* button_;

    QWidget* rightPanel_;
    QVBoxLayout* rightPanelLayout_;
    QPlainTextEdit* appLogWidget_;
    QPalette* appLogWidgetPalette_;

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
    void resetSelectedCompanion(const Companion*);

    void buildSocketInfoWidgets(std::vector<Companion*>*);
    void set();
    void setLeftPanel();
    void addTextToChatHistoryWidget(const QString&);
    void addTextToAppLogWidget(const QString&);
};

MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
