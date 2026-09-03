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

void showErrorDialogAndLogError(QString&&, std::shared_ptr<QWidget>);

class LeftPanelWidget : public QWidget {

    Q_OBJECT

public:
    LeftPanelWidget(std::shared_ptr<QWidget>);
    ~LeftPanelWidget();

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    int getLastCompanionPanelChildWidth();

private:
    std::shared_ptr<QVBoxLayout> layoutPtr_;
    std::shared_ptr<QPalette> palettePtr_;
    std::shared_ptr<QWidget> companionPanelPtr_;
    std::shared_ptr<QVBoxLayout> companionPanelLayoutPtr_;
    std::shared_ptr<QSpacerItem> spacerPtr_;
};

class CentralPanelWidget : public QWidget {

    Q_OBJECT

public:
    CentralPanelWidget(std::shared_ptr<QWidget>, const std::string&);
    ~CentralPanelWidget();

    void set(std::shared_ptr<Companion>);

    void addMessageWidgetToChatHistory(
        std::shared_ptr<WidgetGroup>, std::shared_ptr<Companion>, std::shared_ptr<Message>, std::shared_ptr<MessageState>);

    void scrollDownChatHistory();
    void clearChatHistory();
    void sortChatHistoryElements(bool);

private:
    std::mutex chatHistoryMutex_;
    std::shared_ptr<Companion> companionPtr_;
    std::shared_ptr<QVBoxLayout> layoutPtr_;

    //    std::shared_ptr<QGraphicsScene> graphicsScene_;
    //    std::shared_ptr<QGraphicsView> graphicsView_;
    //    std::shared_ptr<QRectF> rect_;
    //    std::shared_ptr<QGraphicsSimpleTextItem> textItem_;
    //    int linesCount_;
    //    std::shared_ptr<QFont> font_;

    std::shared_ptr<QLabel> companionNameLabelPtr_;
    std::shared_ptr<QPalette> companionNameLabelPalettePtr_;
    std::shared_ptr<QPalette> chatHistoryWidgetPalettePtr_;

    std::shared_ptr<QScrollArea> chatHistoryScrollAreaPtr_;
    // std::shared_ptr<ScrollArea> chatHistoryScrollAreaPtr_;

    std::shared_ptr<QWidget> chatHistoryWidgetPtr_;
    std::shared_ptr<QVBoxLayout> chatHistoryLayoutPtr_;

    std::shared_ptr<QWidget> buttonPanelWidgetPtr_;
    std::shared_ptr<QHBoxLayout> buttonPanelLayoutPtr_;
    std::shared_ptr<QPalette> buttonPanelPalettePtr_;
    std::shared_ptr<QPushButton> sendFileButtonPtr_;

    std::shared_ptr<TextEditWidget> textEditPtr_;
    std::shared_ptr<QPalette> textEditPalettePtr_;

    // std::shared_ptr<QSplitter> splitterPtr_;

    bool eventFilter(std::shared_ptr<QObject>, std::shared_ptr<QEvent>) override;

private slots:
    void sendMessage(const QString&);
    void sendFileSlot();
    void saveFileSlot();
};

class RightPanelWidget : public QWidget {

    Q_OBJECT

public:
    RightPanelWidget(std::shared_ptr<QWidget>);
    ~RightPanelWidget();

    void set();
    void addTextToAppLogWidget(const QString&);

public slots:
    void clearLogAction();

signals:
    void addTextToAppLogWidgetSignal(const QString&);

private:
    std::shared_ptr<QVBoxLayout> layoutPtr_;
    std::shared_ptr<QPlainTextEdit> appLogWidgetPtr_;
    std::shared_ptr<QPalette> appLogWidgetPalettePtr_;

private slots:
    void addTextToAppLogWidgetSlot(const QString&);
    void customMenuRequestedSlot(QPoint);
};

#endif // WIDGETS_PANEL_HPP
