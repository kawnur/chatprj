#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

// #include <map>
#include <memory>
#include <mutex>

#include <QGraphicsBlurEffect>
#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

// class PanelWidget : public QWidget
// {
//     Q_OBJECT

// public:
//     PanelWidget();
//     ~PanelWidget();

// private:

// };

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

template<typename T>
class MainWindowContainerWidget
    : public std::enable_shared_from_this<MainWindowContainerWidget<T>>, public QWidget
{
    // Q_OBJECT

public:
    // MainWindowContainerWidget(QWidgetPtr widget);
    // ~MainWindowContainerWidget() = default;

    // void addWidgetToLayout(QWidgetPtr widget);
    // void addWidgetToLayoutAndSetParentTo(QWidgetPtr widget);
    MainWindowContainerWidget(QWidget *widget) : panel_(nullptr), layout_(nullptr)
    {
        if (widget)
            setParent(widget);
    }

    template<typename...Ts>
    void set(bool init = true, Ts &&...args)
    {
        layout_ = std::make_unique<QVBoxLayout>();
        layout_->setSpacing(0);
        layout_->setContentsMargins(0, 0, 0, 0);
        setLayout(layout_.get());

        if (init) {
            panel_ = std::make_unique<T>(this->shared_from_this(), args...);
            layout_->addWidget(panel_.get());
        }
    }

    void addWidgetToLayout(QWidgetPtr widget)
    {
        if (widget)
            layout_->addWidget(widget.get());
    }

    void addWidgetToLayoutAndSetParentTo(QWidgetPtr widget)
    {
        if (widget) {
            layout_->addWidget(widget.get());
            widget->setParent(this);
        }
    }

    void addTextToAppLogWidget(const QString &text)
    {
        panel_->addTextToAppLogWidget(text);
    }

    void addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget)
    {
        panel_->addWidgetToCompanionPanel(widget);
    }

    void removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget)
    {
        panel_->removeWidgetFromCompanionPanel(widget);
    }

    std::size_t getCompanionPanelChildrenSize()
    {
        return panel_->getCompanionPanelChildrenSize();
    }

    void hide()
    {
        panel_->hide();
    }

    void show()
    {
        panel_->show();
    }

    int getLastCompanionPanelChildWidth()
    {
        return panel_->getLastCompanionPanelChildWidth();
    }

private:
    std::unique_ptr<T> panel_;
    std::unique_ptr<QVBoxLayout> layout_;
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget();
    ~MainWidget() = default;

    // template<typename T>
    // T getContainer(MainWindowContainerPosition position)
    // {
    //     switch (position) {
    //     case MainWindowContainerPosition::LEFT:
    //         return left_;

    //     case MainWindowContainerPosition::CENTRAL:
    //         return central_;

    //     case MainWindowContainerPosition::RIGHT:
    //         return right_;

    //     default:
    //         return nullptr;
    //     }
    // }

    void set();

    std::shared_ptr<QHBoxLayout> layout() const;

    bool addWidgetToContainerAndSetParentTo(
        MainWindowContainerPosition position, QWidgetPtr widget);

    void addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget);
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();

    void initBlurEffect();
    void addTextToAppLogWidget(const QString &text);
    void hideLeftAndRightPanels();
    void showLeftAndRightPanels();

    void hideCentralPanelStub();
    void showCentralPanelStub();
    void hideCompanionPanelStub();
    void showStubPanels();
    void hideStubPanels();
    void enableWidgetsForShowHide();
    void disableWidgetsForShowHide();
    void setBlurEffect();
private:
    std::shared_ptr<QHBoxLayout> layout_;

    std::shared_ptr<MainWindowContainerWidget<LeftPanelWidget>> left_;
    std::shared_ptr<MainWindowContainerWidget<CentralPanelWidget>> central_;
    std::shared_ptr<MainWindowContainerWidget<RightPanelWidget>> right_;

    std::shared_ptr<ShowHideWidget> showHideWidget_;
    std::shared_ptr<QGraphicsBlurEffect> blurEffect_;

    std::unique_ptr<QSplitter> splitter_;
    std::shared_ptr<StubWidgetGroup> stubWidgets_;
};

#endif // WIDGETS_PANEL_HPP
