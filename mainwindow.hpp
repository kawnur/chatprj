#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGridLayout>
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

class TextEditWidget : public QTextEdit {

    Q_OBJECT

public:
    TextEditWidget(MainWindow*);
    ~TextEditWidget() = default;

private:
    void keyPressEvent(QKeyEvent*);
};

class IndicatorWidget : public QWidget {

    Q_OBJECT

public:
    IndicatorWidget(QWidget*);
    IndicatorWidget(const IndicatorWidget*);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();

public slots:
    void toggle();

private:
    bool isOn_;
    QColor onColor_;
    QColor offColor_;
    QPalette* palette_;
};

class SocketInfoWidget : public QWidget {

    Q_OBJECT

public:
//    SocketInfo() = default;
    SocketInfoWidget() {};
    SocketInfoWidget(QString&, QString&, QString&);
    SocketInfoWidget(QString&&, QString&&, QString&&);
//    SocketInfo(const SocketInfo&) {};
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {};
    SocketInfoWidget(std::string&, std::string&, std::string&);
    ~SocketInfoWidget() = default;

    void print();

private:
    QString name_;
    QString ipaddress_;
    QString port_;

    QHBoxLayout* layout_;

    IndicatorWidget* indicator_;
    QLabel* nameLabel_;
    QLabel* ipaddressLabel_;
    QLabel* portLabel_;

    QPushButton* editButton_;
    QPushButton* toggleIndicatorButton_;

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
//    int setClient(EchoClient*);
//    EchoClient* getClient();
    void addText(const QString&);
//    void addMessagesToView();
//    void pushMessage(const QString&);
//    void pushMessageAndAddText(const QString&);
};

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
