#ifndef WIDGETS_PANEL_HPP
#define WIDGETS_PANEL_HPP

#include <memory>
#include <mutex>

#include <QGraphicsBlurEffect>
#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"
#include "widgets.hpp"

class PanelWidget : public VLayoutWidget
{
    Q_OBJECT

public:
    PanelWidget(WidgetType type);
    virtual ~PanelWidget() = default;

    virtual void set() {}
};

class LeftPanelWidget : public PanelWidget
{
    Q_OBJECT

public:
    LeftPanelWidget(QWidgetPtr parent = nullptr);
    ~LeftPanelWidget() = default;

    void addCompanion(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();
    void removeCompanion(SocketInfoBaseWidgetPtr widget);
    int getLastCompanionPanelChildWidth();
    void enable();
    void disable();

private:
    std::unique_ptr<VLayoutWidget> companionPanel_;
};

class CentralPanelWidget : public PanelWidget
{
    Q_OBJECT

public:
    CentralPanelWidget(const std::string &name, bool isStub, QWidgetPtr parent = nullptr);
    ~CentralPanelWidget() = default;

    void set(CompanionPtr companion);
    void set() override {}
    void addMessage(MessageWidgetPtr widget, bool isAntecedent);
    void scrollDownHistory();
    void clearHistory();
    void sortHistory(bool lock);

private:
    std::mutex mutex_;
    CompanionPtr companion_;

    std::unique_ptr<LabelWidget> companionLabel_;
    std::unique_ptr<ScrollAreaWidget> historyScrollArea_;
    std::shared_ptr<VLayoutWidget> history_;
    std::unique_ptr<ButtonPanelWidget> buttons_;
    std::unique_ptr<MessageEditWidget> textEdit_;

    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void sendTextMessageSlot(const QString &text);
    void sendFileSlot();
    void saveFileSlot();
};

class RightPanelWidget : public PanelWidget
{
    Q_OBJECT

public:
    RightPanelWidget(QWidgetPtr parent = nullptr);
    ~RightPanelWidget() = default;

    void set() override;
    void addLog(const QString &text);

public slots:
    void clearLog();

signals:
    void addLogSignal(const QString &text);

private:
    std::unique_ptr<TextEditWidget> log_;

private slots:
    void addLogSlot(const QString &text);
    void customMenuRequestedSlot(const QPoint &position);
};

/*!
 * \brief The CentralPanelContainerWidget class - needed to add center-right splitter
 */
class CentralPanelContainerWidget : public VLayoutWidget
{
public:
    CentralPanelContainerWidget();
    ~CentralPanelContainerWidget() = default;
};

class MainWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MainWidget();
    ~MainWidget() = default;

    bool addWidgetToContainer(MWContainerPosition position, QWidgetPtr widget);
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
    std::shared_ptr<LeftPanelWidget> left_;
    std::shared_ptr<CentralPanelContainerWidget> central_;
    std::shared_ptr<RightPanelWidget> right_;
    std::shared_ptr<ShowHideWidget> showHide_;
    std::shared_ptr<QGraphicsBlurEffect> blur_;
    std::unique_ptr<SplitterWidget> splitter_;
    std::shared_ptr<StubWidgetGroup> stubs_;
};

#endif // WIDGETS_PANEL_HPP
