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


class SocketInfo;
class SocketInfoBaseWidget;
class SocketInfoWidget;
class TextEditWidget;

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

    QLabel* companion_;
    QPlainTextEdit* chatHistoryWidget_;
    QPalette* chatHistoryWidgetPalette_;

    TextEditWidget* textEdit_;
//    QPushButton* button_;

    QWidget* rightPanel_;
    QVBoxLayout* rightPanelLayout_;
    QPlainTextEdit* appLogWidget_;
    QPalette* appLogWidgetPalette_;

//    QPushButton* testPlainTextEditButton_;

//    void buildSockets();

    void addStubWidgetToLeftPanel();
    void addSocketInfoWidgetToLeftPanel(SocketInfo*);

//    void testMainWindowRightPanel();
    void addTestSocketInfoWidgetToLeftPanel();

//    MainWindow(QWidget* parent = nullptr);
//    MainWindow();
    void closeEvent(QCloseEvent*) override;
    virtual ~MainWindow();

    void keyPressEvent(QKeyEvent*) override;

public:
    MainWindow();
//    static MainWindow* instance();

    SocketInfoWidget* selectedSocketInfoWidget_;

    void buildSocketInfoWidgets(std::vector<SocketInfo>*);
//    void set(QString);
    void set();
    void setLeftPanel();
    void setCompanion(QString&);
//    int setClient(EchoClient*);
//    EchoClient* getClient();
    void addTextToChatHistoryWidget(const QString&);
    void addTextToAppLogWidget(const QString&);
//    void addMessagesToView();
//    void pushMessage(const QString&);
//    void pushMessageAndAddText(const QString&);
};

MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
