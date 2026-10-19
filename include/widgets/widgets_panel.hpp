#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

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

    void addCompanion(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();
    void removeCompanion(SocketInfoBaseWidgetPtr widget);
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
    void addMessage(MessageWidgetPtr widget, bool isAntecedent);
    void scrollDownHistory();
    void clearHistory();
    void sortHistory(bool lock);

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

    std::unique_ptr<QLabel> companionLabel_;
    std::unique_ptr<QPalette> companionPalette_;

    std::unique_ptr<QPalette> historyPalette_;
    std::unique_ptr<QScrollArea> historyScrollArea_;
    // std::shared_ptr<ScrollArea> historyScrollArea_;

    QWidgetPtr history_;
    std::unique_ptr<QVBoxLayout> historyLayout_;

    std::unique_ptr<QWidget> buttons_;
    std::unique_ptr<QHBoxLayout> buttonsLayout_;
    std::unique_ptr<QPalette> buttonsPalette_;
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
    void addLog(const QString &text);

public slots:
    void clearLog();

signals:
    void addLogSignal(const QString &text);

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QPlainTextEdit> log_;
    std::unique_ptr<QPalette> palette_;

private slots:
    void addLogSlot(const QString &text);
    void customMenuRequestedSlot(const QPoint &position);
};

template<typename T>
class MainWindowContainerWidget
    : public std::enable_shared_from_this<MainWindowContainerWidget<T>>, public QWidget
{
    // Q_OBJECT

public:
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

    void addWidget(QWidgetPtr widget)
    {
        if (widget) {
            layout_->addWidget(widget.get());
            widget->setParent(this);
        }
    }

    void addLog(const QString &text)
    {
        panel_->addLog(text);
    }

    void addCompanion(SocketInfoBaseWidgetPtr widget)
    {
        panel_->addCompanion(widget);
    }

    void removeCompanion(SocketInfoBaseWidgetPtr widget)
    {
        panel_->removeCompanion(widget);
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

    std::shared_ptr<QHBoxLayout> layout() const;
    void set();

    bool addWidget(MWContainerPosition position, QWidgetPtr widget);
    void addCompanion(SocketInfoBaseWidgetPtr widget);
    void removeCompanion(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();
    void initBlurEffect();
    void addLog(const QString &text);
    void hideLeftAndRight();
    void showLeftAndRight();
    void hideCentralStub();
    void showCentralStub();
    void hideCompanionStub();
    void showStubs();
    void hideStubs();
    void enable();
    void disable();
    void setBlur();

private:
    std::shared_ptr<QHBoxLayout> layout_;
    std::shared_ptr<MainWindowContainerWidget<LeftPanelWidget>> left_;
    std::shared_ptr<MainWindowContainerWidget<CentralPanelWidget>> central_;
    std::shared_ptr<MainWindowContainerWidget<RightPanelWidget>> right_;
    std::shared_ptr<ShowHideWidget> showHide_;
    std::shared_ptr<QGraphicsBlurEffect> blur_;
    std::unique_ptr<QSplitter> splitter_;
    std::shared_ptr<StubWidgetGroup> stubs_;
};

#endif // WIDGETS_PANEL_HPP
