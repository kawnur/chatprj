#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

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

void showErrorDialogAndLogError(const QString&, QWidget* /*= nullptr*/);

class LeftPanelWidget : public QWidget {

    Q_OBJECT

public:
    LeftPanelWidget(QWidget*);
    ~LeftPanelWidget();

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);
    int getLastCompanionPanelChildWidth();

private:
    QVBoxLayout* layoutPtr_;
    QPalette* palettePtr_;
    QWidget* companionPanelPtr_;
    QVBoxLayout* companionPanelLayoutPtr_;
    QSpacerItem* spacerPtr_;
};

class CentralPanelWidget : public QWidget {

    Q_OBJECT

public:
    CentralPanelWidget(QWidget*, const std::string&);
    ~CentralPanelWidget();

    void set(Companion*);

    void addMessageWidgetToChatHistory(
        const WidgetGroup*, Companion*, const Message*, const MessageState*);

    void scrollDownChatHistory();
    void clearChatHistory();
    void sortChatHistoryElements(bool);

private:
    std::mutex chatHistoryMutex_;
    Companion* companionPtr_;
    QVBoxLayout* layoutPtr_;

    //    QGraphicsScene* graphicsScene_;
    //    QGraphicsView* graphicsView_;
    //    QRectF* rect_;
    //    QGraphicsSimpleTextItem* textItem_;
    //    int linesCount_;
    //    QFont* font_;

    QLabel* companionNameLabelPtr_;
    QPalette* companionNameLabelPalettePtr_;
    QPalette* chatHistoryWidgetPalettePtr_;

    QScrollArea* chatHistoryScrollAreaPtr_;
    // ScrollArea* chatHistoryScrollAreaPtr_;

    QWidget* chatHistoryWidgetPtr_;
    QVBoxLayout* chatHistoryLayoutPtr_;

    QWidget* buttonPanelWidgetPtr_;
    QHBoxLayout* buttonPanelLayoutPtr_;
    QPalette* buttonPanelPalettePtr_;
    QPushButton* sendFileButtonPtr_;

    TextEditWidget* textEditPtr_;
    QPalette* textEditPalettePtr_;

    // QSplitter* splitterPtr_;

    bool eventFilter(QObject*, QEvent*) override;

private slots:
    void sendMessage(const QString&);
    void sendFileSlot();
    void saveFileSlot();
};

class RightPanelWidget : public QWidget {

    Q_OBJECT

public:
    RightPanelWidget(QWidget*);
    ~RightPanelWidget();

    void set();
    void addTextToAppLogWidget(const QString&);

public slots:
    void clearLogAction();

signals:
    void addTextToAppLogWidgetSignal(const QString&);

private:
    QVBoxLayout* layoutPtr_;
    QPlainTextEdit* appLogWidgetPtr_;
    QPalette* appLogWidgetPalettePtr_;

private slots:
    void addTextToAppLogWidgetSlot(const QString&);
    void customMenuRequestedSlot(QPoint);
};

#endif // WIDGETS_PANEL_HPP
