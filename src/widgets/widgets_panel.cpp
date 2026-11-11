#include "widgets_panel.hpp"

#include <memory>
#include <string>

#include <QApplication>
#include <QMenu>
#include <QScrollBar>
#include <QWidget>

#include "constants.hpp"
#include "logging.hpp"
#include "graphic_manager.hpp"
#include "message.hpp"  // used in lambda
#include "utils_cout.hpp"
#include "utils_widgets.hpp"
#include "widget_group.hpp"
#include "widgets_message.hpp"

const uint8_t BLUR_RADIUS = 30;
const uint16_t RIGHT_PANEL_FIXED_WIDTH = 100;

PanelWidget::PanelWidget(WidgetType type) : VLayoutWidget(type, Qt::AlignLeft) {}

LeftPanelWidget::LeftPanelWidget(QWidgetPtr parent) : PanelWidget(WidgetType::LEFT_PANEL)
{
    if (parent)
        setParent(parent.get());

    companionPanel_ = std::make_unique<VLayoutWidget>(WidgetType::COMPANION_PANEL, Qt::AlignLeft);
    addWidget(companionPanel_.get());
    layout_->addStretch(1);
}

void LeftPanelWidget::addCompanion(SocketInfoBaseWidgetPtr widget)
{
    companionPanel_->addWidget(widget.get());
}

std::size_t LeftPanelWidget::getCompanionPanelChildrenSize()
{
    // TODO get rid of children mechanism usage
    auto companionPanelChildren =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void LeftPanelWidget::removeCompanion(SocketInfoBaseWidgetPtr widget)
{
    // TODO get rid of children mechanism usage
    auto companionPanelChildren =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    auto index = companionPanelChildren.indexOf(widget.get());

    if (index == -1) {
        // showErrorDialogAndLogError("SocketInfoBaseWidget was not found in companion panel");
    } else if (index == 0) {

    } else if (index > 0) {
        auto previous = companionPanelChildren.at(index - 1);
        getGraphicManager()->resetSelectedCompanion(previous);
        companionPanel_->removeWidget(widget.get());
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth()
{
    // TODO get rid of children mechanism usage
    auto children =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    return runAndReturnDefault([&]() { return children.at(children.size() - 1)->width(); }, -1);
}

void LeftPanelWidget::enable()
{
    companionPanel_->setDisabled(false);
}

void LeftPanelWidget::disable()
{
    companionPanel_->setDisabled(true);
}

CentralPanelWidget::CentralPanelWidget(const std::string &name, bool isStub, QWidgetPtr parent)
    : PanelWidget(WidgetType::CENTRAL_PANEL), mutex_()
{
    // fields
    historyScrollArea_ = nullptr;

    // parent
    if (parent)
        setParent(parent.get());

    companionLabel_ =
        std::make_unique<LabelWidget>(WidgetType::COMPANION_NAME_LABEL, getQString(name));

    addWidget(companionLabel_.get());

    // history
    history_ = std::make_shared<VLayoutWidget>(WidgetType::HISTORY, Qt::AlignLeft);
    history_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    if (name.size() != 0) {
        historyScrollArea_ = std::make_unique<ScrollAreaWidget>(history_);
        addWidget(historyScrollArea_.get());
    }

    if (isStub)
        return;

    // button panel
    std::initializer_list<ButtonInfo> list = {
        { { SEND_FILE_BUTTON_TEXT }, [this]() { sendFileSlot(); } },
        { { SEND_TEXT_BUTTON_TEXT }, [this]() { textEdit_->sendSlot(); } }
    };

    buttons_ = std::make_unique<ButtonPanelWidget>(WidgetType::HISTORY_BUTTON_PANEL, list);
    addWidget(buttons_.get());

    // text edit panel
    textEdit_ = std::make_unique<MessageEditWidget>(WidgetType::TEXT_EDIT);
    addWidget(textEdit_.get());
}

void CentralPanelWidget::set(CompanionPtr companion)
{
    companion_ = companion;

    connect(
        textEdit_.get(), &MessageEditWidget::sendSignal,
        this, &CentralPanelWidget::sendTextMessageSlot, Qt::QueuedConnection);

    historyScrollArea_->installEventFilter(this);
}

void CentralPanelWidget::addMessage(MessageWidgetPtr widget, bool isAntecedent)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        widget->setParent(history_.get());
        history_->addWidget(widget.get());

        if (isAntecedent)
            sortHistory(false);
    }

    scrollDownHistory();
}

void CentralPanelWidget::scrollDownHistory()
{
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    auto value = historyScrollArea_->verticalScrollBar()->maximum();
    historyScrollArea_->verticalScrollBar()->setValue(value);
}

void CentralPanelWidget::clearHistory()
{
    auto children = history_->children();

    for (auto &child : children) {
        auto messageWidget = qobject_cast<MessageWidget *>(child);

        if (messageWidget)
            messageWidget->hide();
    }
}

void CentralPanelWidget::sortHistory(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    QList<MessageWidget *> list;

    for (const auto &item : history_->children()) {
        auto cast = qobject_cast<MessageWidget *>(item);

        if (cast)
            list.push_back(cast);
    }

    auto lambda = [&](const auto &item)
    {
        auto message = item->message();

        return (message) ? message->time() : ""s;
    };

    auto sortingLambda = [&](MessageWidget *element1, MessageWidget *element2)
    {
        auto res1 = lambda(element1);
        auto res2 = lambda(element2);
        bool res = (res1 < res2);

        coutArgsWithSpaceSeparator("res1:", res1, "res2:", res2, "res:", res);

        return res;
    };

    auto print = [&]()
    {
        for (const auto l : list)
            coutArgsWithSpaceSeparator(l->message()->_data());
    };

    coutArgsWithSpaceSeparator("BEFORE SORTING");
    print();

    std::ranges::sort(list, sortingLambda);

    coutArgsWithSpaceSeparator("AFTER SORTING");
    print();


    for (auto &element : list) {
        auto message = element->message();

        coutArgsWithSpaceSeparator("message:", message.get());

        if (message)
            coutArgsWithSpaceSeparator("element message:", message->_data());

        history_->removeWidget(element);
        history_->addWidget(element);
    }
}

bool CentralPanelWidget::eventFilter(QObject *object, QEvent *event)
{
    auto result = QWidget::eventFilter(object, event);

    if (object == historyScrollArea_.get()) {
        auto bar = historyScrollArea_->verticalScrollBar();

        if (bar && bar->value() == bar->minimum()) {
            auto cast = dynamic_cast<QKeyEvent *>(event);
            auto type = event->type();

            bool condition1 = (type == QEvent::Wheel);

            auto keys = (Qt::Key_Up | Qt::Key_PageUp | Qt::Key_Home);
            bool condition21 = (cast && (cast->key() & keys));
            bool condition2 = (type == QEvent::KeyPress && condition21);

            if (condition1 || condition2) {
                logTemplateInfo("scroll bar minimum event type '{}'", getString(event->type()));

                getGraphicManager()->addEarlyMessages(companion_);
            }
        }
    }

    return result;
}

void CentralPanelWidget::sendTextMessageSlot(const QString &text)
{
    if (!text.isEmpty())
        getGraphicManager()->sendTextMessage(companion_, text.toStdString());
}

void CentralPanelWidget::sendFileSlot()
{
    getGraphicManager()->sendFile(companion_);
}

void CentralPanelWidget::saveFileSlot()
{
    // getGraphicManager()->saveFile(companion_);
}

RightPanelWidget::RightPanelWidget(QWidgetPtr parent) : PanelWidget(WidgetType::RIGHT_PANEL)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedWidth(RIGHT_PANEL_FIXED_WIDTH);

    log_ = std::make_unique<TextEditWidget>(WidgetType::APP_LOG, true);

    addWidget(log_.get());

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);
}

void RightPanelWidget::set()
{
    connect(
        this, SIGNAL(addLogSignal(QString)), this, SLOT(addLogSlot(QString)), Qt::QueuedConnection);

    log_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        log_.get(), &QWidget::customContextMenuRequested,
        this, &RightPanelWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void RightPanelWidget::addLog(const QString &text)
{
    emit addLogSignal(text);
}

void RightPanelWidget::clearLog()
{
    log_->clear();
}

void RightPanelWidget::addLogSlot(const QString &text)
{
    log_->append(text);

    QApplication::processEvents();
    QApplication::processEvents();

    log_->ensureCursorVisible();
}

void RightPanelWidget::customMenuRequestedSlot(const QPoint &position)
{
    auto menu = std::make_shared<QMenu>(this);
    auto clearLogAction = std::make_shared<QAction>("Clear log", this);
    menu->addAction(clearLogAction.get());

    connect(
        clearLogAction.get(), &QAction::triggered,
        this, &RightPanelWidget::clearLog, Qt::QueuedConnection);

    menu->popup(mapToGlobal(position));
}

CentralPanelContainerWidget::CentralPanelContainerWidget()
    : VLayoutWidget(WidgetType::CONTAINER_WIDGET, Qt::AlignLeft)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

MainWidget::MainWidget() : HLayoutWidget(WidgetType::UNKNOWN, Qt::AlignLeft)
{
    // left panel
    left_ = std::make_shared<LeftPanelWidget>();
    left_->set();
    addWidget(left_.get());

    // central container
    central_ = std::make_shared<CentralPanelContainerWidget>();
    addWidget(central_.get());

    // right panel
    right_ = std::make_shared<RightPanelWidget>();
    right_->set();
    addWidget(right_.get());

    // splitter
    splitter_ = std::make_unique<SplitterWidget>(WidgetType::SPLITTER_CENTRAL_RIGHT);
    splitter_->addWidget(central_.get());
    splitter_->addWidget(right_.get());
    addWidget(splitter_.get());

    // stub widgets
    stubs_ = std::make_shared<StubWidgetGroup>();
    addCompanion(stubs_->socketInfoBase());
    left_->addWidget(stubs_->left().get());
    addWidgetToCentralContainer(stubs_->central());
    right_->addWidget(stubs_->right().get());
    stubs_->hide();
    stubs_->showCentral();

    // show - hide widget
    showHide_ = std::make_shared<ShowHideWidget>();
    left_->addWidget(showHide_.get());

    // blur effect
    initBlurEffect();
}

void MainWidget::addWidgetToCentralContainer(QWidgetPtr widget)
{
    central_->addWidget(widget.get());
}

void MainWidget::addCompanion(SocketInfoBaseWidgetPtr widget)
{
    left_->addCompanion(widget);
}

void MainWidget::removeCompanion(SocketInfoBaseWidgetPtr widget)
{
    left_->removeCompanion(widget);
}

std::size_t MainWidget::getCompanionPanelChildrenSize()
{
    return left_->getCompanionPanelChildrenSize();
}

void MainWidget::initBlurEffect()
{
    blur_ = std::make_shared<QGraphicsBlurEffect>();
    blur_->setBlurRadius(BLUR_RADIUS);
}

void MainWidget::addLog(const QString &text)
{
    right_->addLog(text);
}

void MainWidget::hideLeftAndRight()
{
    left_->hide();
    right_->hide();
}

void MainWidget::showLeftAndRight()
{
    left_->show();
    right_->show();
}

void MainWidget::hideCentralStub()
{
    stubs_->hideCentral();
}

void MainWidget::showCentralStub()
{
    stubs_->showCentral();
}

void MainWidget::hideCompanionStub()
{
    stubs_->hideSocketInfo();
}

void MainWidget::showStubs()
{
    hideLeftAndRight();
    stubs_->setLeftPanelWidth(left_->getLastCompanionPanelChildWidth());
    stubs_->show();
}

void MainWidget::hideStubs()
{
    showLeftAndRight();
    stubs_->hide();
}

void MainWidget::enable()
{
    left_->enable();
    central_->setEnabled(true);
    right_->setEnabled(true);
}

void MainWidget::disable()
{
    left_->disable();  // leave showHide enabled
    central_->setEnabled(false);
    right_->setEnabled(false);
}

void MainWidget::setBlur()
{
    setGraphicsEffect(blur_.get());
}
