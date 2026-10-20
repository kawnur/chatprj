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
#include "widgets.hpp"
#include "widgets_message.hpp"

LeftPanelWidget::LeftPanelWidget(QWidgetPtr parent)
{
    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_.get());

    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Window, QColor(getWidgetColor(Widget::LEFT_PANEL)));
    setAutoFillBackground(true);
    setPalette(*palette_);

    companionPanel_ = std::make_unique<QWidget>();
    // leftPanel_->setStyleSheet("border-right: 1px solid black");
    companionPanel_->resize(2000, 1000);  // TODO ???
    companionPanelLayout_ = std::make_unique<QVBoxLayout>();
    companionPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    companionPanelLayout_->setSpacing(0);
    companionPanelLayout_->setContentsMargins(0, 0, 0, 0);
    companionPanel_->setLayout(companionPanelLayout_.get());

    layout_->addWidget(companionPanel_.get());

    companionPanel_->resize(4000, 1000);  // TODO ???

    spacer_ = std::make_unique<QSpacerItem>(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout_->addSpacerItem(spacer_.get());
}

void LeftPanelWidget::addCompanion(SocketInfoBaseWidgetPtr widget)
{
    companionPanelLayout_->addWidget(widget.get());
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
        companionPanelLayout_->removeWidget(widget.get());
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth()
{
    // TODO get rid of children mechanism usage
    auto children =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    return runAndReturnDefault([&]() { return children.at(children.size() - 1)->width(); }, -1);
}

CentralPanelWidget::CentralPanelWidget(QWidgetPtr parent, const std::string &name) : mutex_()
{
    historyScrollArea_ = nullptr;
    historyPalette_ = nullptr;

    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());

    companionLabel_ = std::make_unique<QLabel>(getQString(name));
    // companionNameLabel_->setStyleSheet("border-bottom: 1px solid black");
    companionPalette_ = std::make_unique<QPalette>();
    auto color = QColor(getWidgetColor(Widget::COMPANION_NAME_LABEL));
    companionPalette_->setColor(QPalette::Window, color);
    companionLabel_->setAutoFillBackground(true);
    companionLabel_->setPalette(*companionPalette_);

    layout_->addWidget(companionLabel_.get());

    history_ = std::make_shared<QWidget>();

    historyLayout_ = std::make_unique<QVBoxLayout>();
    historyLayout_->setSpacing(0);
    historyLayout_->setContentsMargins(0, 0, 0, 0);
    historyLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    historyLayout_->setSizeConstraint(QLayout::SetMaximumSize);
    history_->setLayout(historyLayout_.get());

    if (name.size() != 0) {
        historyScrollArea_ = std::make_unique<QScrollArea>();
        // chatHistoryScrollArea_ = new ScrollArea;
        historyScrollArea_->setWidgetResizable(true);
        historyScrollArea_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        historyScrollArea_->setWidget(history_.get());
        historyPalette_ = std::make_unique<QPalette>();
        historyPalette_->setColor(QPalette::Window, QColorConstants::Gray);
        history_->setPalette(*historyPalette_);

        layout_->addWidget(historyScrollArea_.get());
    }

    buttons_ = std::make_unique<QWidget>();
    buttonsLayout_ = std::make_unique<QHBoxLayout>();
    buttonsLayout_->setSpacing(10);
    buttonsLayout_->setContentsMargins(10, 10, 10, 10);
    buttonsLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buttons_->setLayout(buttonsLayout_.get());
    buttonsPalette_ = std::make_unique<QPalette>();
    buttonsPalette_->setColor(QPalette::Window, QColor(getWidgetColor(Widget::BUTTON_PANEL)));
    history_->setPalette(*buttonsPalette_);
    sendFileButton_ = std::make_unique<QPushButton>(getButtonText(ButtonType::SEND_FILE));
    sendTextButton_ = std::make_unique<QPushButton>(getButtonText(ButtonType::SEND_TEXT));
    buttonsLayout_->addWidget(sendFileButton_.get());
    buttonsLayout_->addWidget(sendTextButton_.get());

    layout_->addWidget(buttons_.get());

    textEdit_ = std::make_unique<TextEditWidget>();
    layout_->addWidget(textEdit_.get());

    // splitter_ = new QSplitter;
    // layout_->addWidget(splitter_);
    // splitter_->addWidget(borderWidget_);
    // splitter_->addWidget(textEdit_);
}

void CentralPanelWidget::set(CompanionPtr companion)
{
    companion_ = companion;

    connect(
        textEdit_.get(), &TextEditWidget::sendSignal,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        sendFileButton_.get(), &QPushButton::clicked,
        this, &CentralPanelWidget::sendFileSlot, Qt::QueuedConnection);

    connect(
        sendTextButton_.get(), &QPushButton::clicked,
        textEdit_.get(), &TextEditWidget::sendSlot, Qt::QueuedConnection);

    historyScrollArea_->installEventFilter(this);
}

void CentralPanelWidget::addMessage(MessageWidgetPtr widget, bool isAntecedent)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        widget->setParent(history_.get());

        historyLayout_->addWidget(widget.get());

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

    auto list = history_->children();

    auto lambda = [&](const auto &item)
    {
        auto cast = qobject_cast<MessageWidget *>(item);
        auto message = cast->message();

        return (message) ? message->time() : ""s;
    };

    auto sortingLambda = [&](QObject *element1, QObject *element2)
    {
        auto res1 = lambda(element1);
        auto res2 = lambda(element2);
        bool res = (res1 < res2);

        coutArgsWithSpaceSeparator("res1:", res1, "res2:", res2, "res:", res);

        return res;
    };

    std::ranges::sort(list, sortingLambda);

    coutArgsWithSpaceSeparator("AFTER SORTING");

    for (auto &element : list) {
        auto cast = qobject_cast<MessageWidget *>(element);
        auto message = cast->message();

        coutArgsWithSpaceSeparator("message:", message.get());

        if (message)
            coutArgsWithSpaceSeparator("element message:", message->text());

        historyLayout_->removeWidget(cast);
        historyLayout_->addWidget(cast);
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
                logTemplateInfo("scroll bar minimum event type {}", getString(event->type()));

                getGraphicManager()->addEarlyMessages(companion_);
            }
        }
    }

    return result;
}

void CentralPanelWidget::sendMessage(const QString &text)
{
    if (!text.isEmpty())
        getGraphicManager()->sendMessage(MessageType::TEXT, companion_, text.toStdString());
}

void CentralPanelWidget::sendFileSlot()
{
    getGraphicManager()->sendFile(companion_);
}

void CentralPanelWidget::saveFileSlot()
{
    // getGraphicManager()->saveFile(companion_);
}

RightPanelWidget::RightPanelWidget(QWidgetPtr parent)
{
    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());

    log_ = std::make_unique<QPlainTextEdit>();
    // appLogWidget_->setStyleSheet("border-left: 1px solid black");
    log_->setReadOnly(true);
    log_->setPlainText("");

    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Base, QColor(getWidgetColor(Widget::APP_LOG)));
    palette_->setColor(QPalette::Text, QColorConstants::Black);
    log_->setAutoFillBackground(true);
    log_->setPalette(*palette_);

    layout_->addWidget(log_.get());

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
    log_->setParent(this);

    connect(
        this, SIGNAL(addLogSignal(const QString &)), this, SLOT(addLogSlot(const QString &)),
        Qt::QueuedConnection);

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
    log_->appendPlainText(text);

    // QApplication::processEvents();

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

MainWidget::MainWidget() : layout_(nullptr) {}

void MainWidget::set()
{
    // layout
    layout_ = std::make_unique<QHBoxLayout>(this);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());

    // left container
    left_ = std::make_shared<MainWindowContainerWidget<LeftPanelWidget>>(this);
    left_->set();
    layout_->addWidget(left_.get());

    // central container
    central_ = std::make_shared<MainWindowContainerWidget<CentralPanelWidget>>(this);
    central_->set(false, ""s);
    layout_->addWidget(central_.get());

    // right container
    right_ = std::make_shared<MainWindowContainerWidget<RightPanelWidget>>(this);
    right_->set();
    layout_->addWidget(right_.get());

    // splitter
    splitter_ = std::make_unique<QSplitter>(this);
    splitter_->addWidget(central_.get());
    splitter_->addWidget(right_.get());
    layout_->addWidget(splitter_.get());

    // stub widgets
    stubs_ = std::make_shared<StubWidgetGroup>();
    stubs_->setParents(left_, central_);
    addCompanion(stubs_->socketInfoBase());
    addWidget(MWContainerPosition::LEFT, stubs_->left());
    addWidget(MWContainerPosition::CENTRAL, stubs_->central());
    addWidget(MWContainerPosition::RIGHT, stubs_->right());
    stubs_->hide();
    stubs_->showCentral();

    // show - hide widget
    if (!addWidget(MWContainerPosition::LEFT, showHide_))
        logArgsError("addWidgetToContainerAndSetParentTo error");

    // blur effect
    initBlurEffect();
}

std::shared_ptr<QHBoxLayout> MainWidget::layout() const
{
    return layout_;
}

bool MainWidget::addWidget(MWContainerPosition position, QWidgetPtr widget)
{
    switch (position) {
    case MWContainerPosition::LEFT:
        left_->addWidget(widget);

    break;

    case MWContainerPosition::CENTRAL:
        central_->addWidget(widget);

    break;

    case MWContainerPosition::RIGHT:
        right_->addWidget(widget);

    break;

    default:
        return false;
    }

    return true;
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
    left_->setEnabled(true);
    central_->setEnabled(true);
    right_->setEnabled(true);
}

void MainWidget::disable()
{
    left_->setEnabled(false);
    central_->setEnabled(false);
    right_->setEnabled(false);
}

void MainWidget::setBlur()
{
    setGraphicsEffect(blur_.get());
}
