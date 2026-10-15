#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

#include <memory>
#include <mutex>

#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

#include "alias.hpp"

class LeftPanelWidget : public QWidget
{
    Q_OBJECT

public:
    LeftPanelWidget(QWidgetPtr parent);
    ~LeftPanelWidget() = default;

    void addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget);
    int getLastCompanionPanelChildWidth();

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QPalette> palette_;
    std::unique_ptr<QWidget> companionPanel_;
    std::unique_ptr<QVBoxLayout> companionPanelLayout_;
    std::unique_ptr<QSpacerItem> spacer_;
};

class CentralPanelWidget : public QWidget
{
    Q_OBJECT

public:
    CentralPanelWidget(QWidgetPtr parent, const std::string &name);
    ~CentralPanelWidget() = default;

    void set(CompanionPtr companion);

    // MessageWidgetPtr buildMessageWidget(
    //     CompanionPtr companion, MessagePtr message);

    void addMessageWidget(
        // WidgetGroupPtr widgetGroup, CompanionPtr companion,
        // MessagePtr message);
        MessageWidgetPtr widget, bool isAntecedent);

    void scrollDownChatHistory();
    void clearChatHistory();
    void sortChatHistory(bool lock);

private:
    std::mutex mutex_;
    CompanionPtr companion_;
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

    QWidgetPtr chatHistoryWidget_;
    std::unique_ptr<QVBoxLayout> chatHistoryLayout_;

    std::unique_ptr<QWidget> buttonPanelWidget_;
    std::unique_ptr<QHBoxLayout> buttonPanelLayout_;
    std::unique_ptr<QPalette> buttonPanelPalette_;
    std::unique_ptr<QPushButton> sendFileButton_;
    std::unique_ptr<QPushButton> sendTextButton_;

    std::unique_ptr<TextEditWidget> textEdit_;
    std::unique_ptr<QPalette> textEditPalette_;

    // std::shared_ptr<QSplitter> splitter_;

    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void sendMessage(const QString &text);
    void sendFileSlot();
    void saveFileSlot();
};

class RightPanelWidget : public QWidget
{
    Q_OBJECT

public:
    RightPanelWidget(QWidgetPtr parent);
    ~RightPanelWidget() = default;

    void set();
    void addTextToAppLogWidget(const QString &text);

public slots:
    void clearLogAction();

signals:
    void addTextToAppLogWidgetSignal(const QString &text);

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QPlainTextEdit> appLogWidget_;
    std::unique_ptr<QPalette> appLogWidgetPalette_;

private slots:
    void addTextToAppLogWidgetSlot(const QString &text);
    void customMenuRequestedSlot(const QPoint &position);
};

#endif // WIDGETS_PANEL_HPP
