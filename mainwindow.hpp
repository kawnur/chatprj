#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRectF>
#include <QScrollBar>
#include <QString>
#include <QTextEdit>
#include <QTimer>
#include <queue>
#include <QVBoxLayout>
#include <string>
#include <thread>

#include "application.hpp"
//#include "blocking_tcp_echo_client.hpp"
//#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"


class MainWindow;

MainWindow* getMainWindowPtr();

class TextEdit : public QTextEdit {

    Q_OBJECT

public:
    TextEdit(MainWindow*);
    ~TextEdit() = default;

private:
    void keyPressEvent(QKeyEvent*);
};

class SocketInfo : public QWidget {

    Q_OBJECT
//class SocketInfo {

public:
//    SocketInfo() = default;
    SocketInfo() {};
    SocketInfo(QString&, QString&, QString&);
    SocketInfo(QString&&, QString&&, QString&&);
//    SocketInfo(const SocketInfo&) {};
    SocketInfo(const SocketInfo&);
    SocketInfo(SocketInfo&&) {};
    SocketInfo(std::string&, std::string&, std::string&);
    ~SocketInfo() = default;

    void print();

    QString name_;
    QString ipaddress_;
    QString port_;

private:
    QHBoxLayout* layout_;
    QLabel* nameLabel_;
    QLabel* ipaddressLabel_;
    QLabel* portLabel_;

    QPushButton* editButton_;

    void set();

};

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
    std::vector<SocketInfo> sockets_;

    QWidget* centralPanel_;
    QVBoxLayout* centralPanelLayout_;
    QGraphicsScene* graphicsScene_;
    QGraphicsView* graphicsView_;
    QRectF* rect_;
    QGraphicsSimpleTextItem* textItem_;
    int linesCount_;
    QFont* font_;
    TextEdit* textEdit_;
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
//    int setClient(EchoClient*);
//    EchoClient* getClient();
    void addText(const QString&);
//    void addMessagesToView();
//    void pushMessage(const QString&);
//    void pushMessageAndAddText(const QString&);
};

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
