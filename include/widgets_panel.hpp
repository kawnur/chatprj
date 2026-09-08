#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

#include <memory>
#include <mutex>

#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

#include "constants.hpp"
#include "manager.hpp"
#include "utils.hpp"
#include "widgets.hpp"

class Companion;
class Message;
class MessageState;
class SocketInfoBaseWidget;
class TextEditWidget;
class WidgetGroup;

// void showErrorDialogAndLogError(QString&&, std::shared_ptr<QWidget>);

class LeftPanelWidget : public QWidget {

    Q_OBJECT

public:
    LeftPanelWidget(std::shared_ptr<QWidget>);
    ~LeftPanelWidget() = default;

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    int getLastCompanionPanelChildWidth();

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QPalette> palette_;
    std::unique_ptr<QWidget> companionPanel_;
    std::unique_ptr<QVBoxLayout> companionPanelLayout_;
    std::unique_ptr<QSpacerItem> spacer_;
};

class CentralPanelWidget : public QWidget {

    Q_OBJECT

public:
    CentralPanelWidget(std::shared_ptr<QWidget>, const std::string&);
    ~CentralPanelWidget() = default;

    void set(std::shared_ptr<Companion>);

    void addMessageWidgetToChatHistory(
        std::shared_ptr<WidgetGroup>, std::shared_ptr<Companion>, std::shared_ptr<Message>, std::shared_ptr<MessageState>);

    void scrollDownChatHistory();
    void clearChatHistory();
    void sortChatHistoryElements(bool);

private:
    std::mutex chatHistoryMutex_;
    std::shared_ptr<Companion> companion_;
    std::unique_ptr<QVBoxLayout> layout_;

    //    std::shared_ptr<QGraphicsScene> graphicsScene_;
    //    std::shared_ptr<QGraphicsView> graphicsView_;
    //    std::shared_ptr<QRectF> rect_;
    //    std::shared_ptr<QGraphicsSimpleTextItem> textItem_;
    //    int linesCount_;
    //    std::shared_ptr<QFont> font_;

    std::unique_ptr<QLabel> companionNameLabel_;
    std::unique_ptr<QPalette> companionNameLabelPalette_;
    std::unique_ptr<QPalette> chatHistoryWidgetPalette_;

    std::unique_ptr<QScrollArea> chatHistoryScrollArea_;
    // std::shared_ptr<ScrollArea> chatHistoryScrollArea_;

    std::unique_ptr<QWidget> chatHistoryWidget_;
    std::unique_ptr<QVBoxLayout> chatHistoryLayout_;

    std::unique_ptr<QWidget> buttonPanelWidget_;
    std::unique_ptr<QHBoxLayout> buttonPanelLayout_;
    std::unique_ptr<QPalette> buttonPanelPalette_;
    std::unique_ptr<QPushButton> sendFileButton_;

    std::unique_ptr<TextEditWidget> textEdit_;
    std::unique_ptr<QPalette> textEditPalette_;

    // std::shared_ptr<QSplitter> splitter_;

    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void sendMessage(const QString&);
    void sendFileSlot();
    void saveFileSlot();
};

class RightPanelWidget : public QWidget {

    Q_OBJECT

public:
    RightPanelWidget(std::shared_ptr<QWidget>);
    ~RightPanelWidget() = default;

    void set();
    void addTextToAppLogWidget(const QString&);

public slots:
    void clearLogAction();

signals:
    void addTextToAppLogWidgetSignal(const QString&);

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QPlainTextEdit> appLogWidget_;
    std::unique_ptr<QPalette> appLogWidgetPalette_;

private slots:
    void addTextToAppLogWidgetSlot(const QString&);
    void customMenuRequestedSlot(QPoint);
};

#endif // WIDGETS_PANEL_HPP
