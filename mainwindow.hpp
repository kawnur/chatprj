#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRectF>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <queue>
#include <QVBoxLayout>
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

class MainWindow : public QMainWindow {

    Q_OBJECT

private:
//    static MainWindow* _instance;

//    QString role_;

//    std::queue<QString> messages_;

//    EchoClient* client_;

    QWidget* centralWidget_;
    QVBoxLayout* centralWidgetLayout_;
    QGraphicsScene* graphicsScene_;
    QGraphicsView* graphicsView_;
    QRectF* rect_;
    QGraphicsSimpleTextItem* textItem_;
    int linesCount_;
    QFont* font_;
    TextEdit* textEdit_;
//    QPushButton* button_;

//    MainWindow(QWidget* parent = nullptr);
//    MainWindow();
    void closeEvent(QCloseEvent*);
    virtual ~MainWindow() = default;

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
