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
    QGraphicsScene* graphicsScene_;
    QGraphicsView* graphicsView_;
    QRectF* rect_;
    QGraphicsSimpleTextItem* textItem_;
    int linesCount_;
    QFont* font_;
    TextEditWidget* textEdit_;
//    QPushButton* button_;

    QWidget* rightPanel_;
    QVBoxLayout* rightPanelLayout_;
    QPlainTextEdit* plainTextEdit_;
    QPalette* plainTextEditPalette_;

//    QPushButton* testPlainTextEditButton_;

//    void buildSockets();

    void addStubWidgetToLeftPanel();

//    void testMainWindowRightPanel();

//    MainWindow(QWidget* parent = nullptr);
//    MainWindow();
    void closeEvent(QCloseEvent*);
    virtual ~MainWindow();

public:
    MainWindow();
//    static MainWindow* instance();

    void buildSocketInfoWidgets(std::vector<SocketInfo>*);
//    void set(QString);
    void set();
    void setLeftPanel();
//    void connectToDb();
//    int setClient(EchoClient*);
//    EchoClient* getClient();
    void addTextToCentralPanel(const QString&);
    void addTextToRightPanel(const QString&);
//    void addMessagesToView();
//    void pushMessage(const QString&);
//    void pushMessageAndAddText(const QString&);
};

MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
