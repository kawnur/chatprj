#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QScrollBar>
#include <QVBoxLayout>

#include "application.hpp"
//#include "blocking_tcp_echo_client.hpp"
#include "db_interaction.hpp"
//#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"
#include "widgets.hpp"


MainWindow* getMainWindowPtr();

class SocketInfoWidget;
class TextEditWidget;

class MainWindow : public QMainWindow {

    Q_OBJECT

private:
//    static MainWindow* _instance;

//    QString role_;

//    std::queue<QString> messages_;

//    EchoClient* client_;

    QWidget* centralWidget_;
    QHBoxLayout* centralWidgetLayout_;

    QWidget* leftPanel_;
    QVBoxLayout* leftPanelLayout_;
//    QLineEdit* test_;
    std::vector<SocketInfoWidget> sockets_;

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

    QVBoxLayout* rightPanelLayout_;

    void buildSockets();
    void setLeftPanel();    

//    MainWindow(QWidget* parent = nullptr);
//    MainWindow();
    void closeEvent(QCloseEvent*);
    virtual ~MainWindow();

public:
    MainWindow();
//    static MainWindow* instance();

//    void set(QString);
    void set();
    void connectToDb();
//    int setClient(EchoClient*);
//    EchoClient* getClient();
    void addText(const QString&);
//    void addMessagesToView();
//    void pushMessage(const QString&);
//    void pushMessageAndAddText(const QString&);
};

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
