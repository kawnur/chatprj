#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "mainwindow.hpp"
#include "manager.hpp"
#include "logging.hpp"

class Companion;
class Message;
class MainWindow;

const QString CONNECT_BUTTON_CONNECT_LABEL { "Connect" };
const QString CONNECT_BUTTON_DISCONNECT_LABEL { "Disconnect" };

const std::vector<QString> connectButtonLabels
{
    CONNECT_BUTTON_CONNECT_LABEL,
    CONNECT_BUTTON_DISCONNECT_LABEL
};

QString getInitialConnectButtonLabel();
QString getNextConnectButtonLabel();

class TextEditWidget : public QTextEdit
{
    Q_OBJECT

public:
    TextEditWidget() = default;
    ~TextEditWidget() = default;

private:
    void keyPressEvent(QKeyEvent*);

signals:
    void send();
};

class IndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    IndicatorWidget();
    IndicatorWidget(const IndicatorWidget*);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();
    void setMe();

public slots:
    void toggle();

private:
    bool isOn_;
    QColor onColor_;
    QColor offColor_;
    QColor meColor_;
    QPalette* palettePtr_;
};

class SocketInfoBaseWidget : public QWidget
{
    Q_OBJECT

public:
    SocketInfoBaseWidget() = default;
    SocketInfoBaseWidget(const SocketInfoBaseWidget&) = default;
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    void initializeFields();  // non-virtual because is called from constructor
};

class SocketInfoWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoWidget();
    // SocketInfoWidget(QString&, QString&, QString&);
    // SocketInfoWidget(QString&&, QString&&, QString&&);
//    SocketInfo(const SocketInfo&) {};
    SocketInfoWidget(const SocketInfoWidget&);
    SocketInfoWidget(SocketInfoWidget&&) {};
    SocketInfoWidget(std::string&, std::string&, uint16_t&, uint16_t&);
    SocketInfoWidget(std::string&&, std::string&&, uint16_t&&, uint16_t&&);
    ~SocketInfoWidget();

    void print();
    bool isStub() override;

    void select();
    void unselect();

public slots:
    void clientAction();

private:
    Companion* companionPtr_;

    QString name_;
    QString ipAddress_;
    uint16_t serverPort_;
    uint16_t clientPort_;

    QColor selectedColor_;
    QColor unselectedColor_;
    QPalette* palettePtr_;

    QHBoxLayout* layoutPtr_;

    IndicatorWidget* indicatorPtr_;
    QLabel* nameLabelPtr_;
    QLabel* ipAddressLabelPtr_;
    QLabel* serverPortLabelPtr_;
    QLabel* clientPortLabelPtr_;
    QPushButton* editButtonPtr_;
    QPushButton* connectButtonPtr_;
//    QPushButton* toggleIndicatorButton_;

    void initializeFields();

    void changeColor(QColor&);

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
};

class SocketInfoStubWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoStubWidget();
    ~SocketInfoStubWidget() = default;

    bool isStub() override;

private:
    QString mark_;
    QHBoxLayout* layoutPtr_;
    QLabel* markLabelPtr_;

    void initializeFields();
};

class WidgetGroup : public QObject
{
    Q_OBJECT

public:
    WidgetGroup() = default;
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
    // void connectToServer();

private:
    QString buildChatHistory(const Companion*);
};

// class InputFormElementWidget : public QWidget
// {
//     Q_OBJECT

// public:
//     InputFormElementWidget(std::string&&);
//     ~InputFormElementWidget();

// private:
//     std::string label_;

//     QHBoxLayout* layoutPtr_;

//     QLabel* labelPtr_;
//     QLineEdit* editPtr_;
// };

class NewCompanionDialog : public QDialog
{
    Q_OBJECT

public:
    NewCompanionDialog(QWidget*);
    ~NewCompanionDialog();

private:
    QFormLayout* layoutPtr_;

    QLabel* nameLabelPtr_;
    QLineEdit* nameEditPtr_;

    QLabel* ipAddressLabelPtr_;
    QLineEdit* ipAddressEditPtr_;

    QLabel* portLabelPtr_;
    QLineEdit* portEditPtr_;

    QDialogButtonBox* buttonBoxPtr_;

    void sendData();

};


#endif // WIDGETS_HPP
