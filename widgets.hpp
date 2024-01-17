#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

#include "mainwindow.hpp"
#include "logging.hpp"


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

class SocketInfoBaseWidget : public QWidget {

    Q_OBJECT

public:
    SocketInfoBaseWidget() = default;
    SocketInfoBaseWidget(const SocketInfoBaseWidget&) {};
    virtual ~SocketInfoBaseWidget() {};

    virtual bool isStub() {};
    virtual void set() {};
};

class SocketInfoWidget : public SocketInfoBaseWidget {

    Q_OBJECT

public:
    SocketInfoWidget();
    SocketInfoWidget(QString&, QString&, QString&);
    SocketInfoWidget(QString&&, QString&&, QString&&);
//    SocketInfo(const SocketInfo&) {};
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {};
    SocketInfoWidget(std::string&, std::string&, std::string&);
    ~SocketInfoWidget() {};

    void print();
    bool isStub() override;

    void unselect();

private:
    QString name_;
    QString ipaddress_;
    QString port_;

    QColor selectedColor_;
    QColor unselectedColor_;
    QPalette* palette_;

    QHBoxLayout* layout_;

    IndicatorWidget* indicator_;
    QLabel* nameLabel_;
    QLabel* ipaddressLabel_;
    QLabel* portLabel_;
    QPushButton* editButton_;
//    QPushButton* toggleIndicatorButton_;

    void set() override;

    void changeColor(QColor&);
    void select();

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
};

class SocketInfoStubWidget : public SocketInfoBaseWidget {

    Q_OBJECT

public:
    SocketInfoStubWidget();
    ~SocketInfoStubWidget() {};

    bool isStub() override;

private:
    QString mark_;

    QHBoxLayout* layout_;

    QLabel* markLabel_;

    void set() override;
};

#endif // WIDGETS_HPP
