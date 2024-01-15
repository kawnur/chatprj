#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

#include "mainwindow.hpp"


class MainWindow;

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
    virtual ~SocketInfoWidget() = default;

    void print();
    virtual bool isStub();

protected:
    SocketInfoWidget(const QString&);

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
//    QPushButton* toggleIndicatorButton_;

    void set();
};

class SocketInfoStubWidget : public SocketInfoWidget {

    Q_OBJECT

public:
    SocketInfoStubWidget() : SocketInfoWidget("No socket info from DB...") {};
    ~SocketInfoStubWidget() = default;

    bool isStub();
};

#endif // WIDGETS_HPP
