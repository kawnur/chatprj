#include "widgets_panel.hpp"

#include <memory>
#include <string>

#include <QApplication>
#include <QMenu>
#include <QScrollBar>
#include <QWidget>

#include "companion.hpp"
#include "constants.hpp"
#include "logging.hpp"
#include "graphic_manager.hpp"
#include "manager.hpp"
#include "message.hpp"
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

void LeftPanelWidget::addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget)
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

void LeftPanelWidget::removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget)
{
    // TODO get rid of children mechanism usage
    auto companionPanelChildren =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    auto index = companionPanelChildren.indexOf(widget.get());

    if (index == -1) {
        // showErrorDialogAndLogError("SocketInfoBaseWidget was not found in companion panel");
    } else if (index == 0) {

    } else if (index > 0) {
        ManagerPtr manager = getManager();
        auto previousWidget = companionPanelChildren.at(index - 1);
        auto previousCompanion = getGraphicManager()->getMappedCompanionBySocketInfoBaseWidget(previousWidget);
        manager->resetSelectedCompanion(previousCompanion);
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
    chatHistoryScrollArea_ = nullptr;
    chatHistoryWidgetPalette_ = nullptr;

    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());

    companionNameLabel_ = std::make_unique<QLabel>(getQString(name));
    // companionNameLabel_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalette_ = std::make_unique<QPalette>();
    auto color = QColor(getWidgetColor(Widget::COMPANION_NAME_LABEL));
    companionNameLabelPalette_->setColor(QPalette::Window, color);
    companionNameLabel_->setAutoFillBackground(true);
    companionNameLabel_->setPalette(*companionNameLabelPalette_);

    layout_->addWidget(companionNameLabel_.get());

    chatHistoryWidget_ = std::make_shared<QWidget>();

    chatHistoryLayout_ = std::make_unique<QVBoxLayout>();
    chatHistoryLayout_->setSpacing(0);
    chatHistoryLayout_->setContentsMargins(0, 0, 0, 0);
    chatHistoryLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    chatHistoryLayout_->setSizeConstraint(QLayout::SetMaximumSize);
    chatHistoryWidget_->setLayout(chatHistoryLayout_.get());

    if (name.size() != 0) {
        chatHistoryScrollArea_ = std::make_unique<QScrollArea>();
        // chatHistoryScrollArea_ = new ScrollArea;
        chatHistoryScrollArea_->setWidgetResizable(true);
        chatHistoryScrollArea_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        chatHistoryScrollArea_->setWidget(chatHistoryWidget_.get());
        chatHistoryWidgetPalette_ = std::make_unique<QPalette>();
        chatHistoryWidgetPalette_->setColor(QPalette::Window, QColorConstants::Gray);
        chatHistoryWidget_->setPalette(*chatHistoryWidgetPalette_);

        layout_->addWidget(chatHistoryScrollArea_.get());
    }

    buttonPanelWidget_ = std::make_unique<QWidget>();
    buttonPanelLayout_ = std::make_unique<QHBoxLayout>();
    buttonPanelLayout_->setSpacing(10);
    buttonPanelLayout_->setContentsMargins(10, 10, 10, 10);
    buttonPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buttonPanelWidget_->setLayout(buttonPanelLayout_.get());
    buttonPanelPalette_ = std::make_unique<QPalette>();
    buttonPanelPalette_->setColor(QPalette::Window, QColor(getWidgetColor(Widget::BUTTON_PANEL)));
    chatHistoryWidget_->setPalette(*buttonPanelPalette_);
    sendFileButton_ = std::make_unique<QPushButton>(getButtonText(ButtonType::SEND_FILE));
    sendTextButton_ = std::make_unique<QPushButton>(getButtonText(ButtonType::SEND_TEXT));
    buttonPanelLayout_->addWidget(sendFileButton_.get());
    buttonPanelLayout_->addWidget(sendTextButton_.get());

    layout_->addWidget(buttonPanelWidget_.get());

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

    chatHistoryScrollArea_->installEventFilter(this);
}

// MessageWidgetPtr CentralPanelWidget::buildMessageWidget(
//     CompanionPtr companion, MessagePtr message)
// {
//     switch (message->type()) {
//     case MessageType::TEXT:
//         return std::make_shared<TextMessageWidget>(chatHistoryWidget_, companion, message);

//     case MessageType::FILE:
//         return std::make_shared<FileMessageWidget>(chatHistoryWidget_, companion, message);

//     default:
//         logArgsError("message widget building error");

//         return nullptr;
//     }
// }

void CentralPanelWidget::addMessageWidget(
    // WidgetGroupPtr widgetGroup, CompanionPtr companion,
    // MessagePtr message)
    MessageWidgetPtr widget, bool isAntecedent)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // auto widget = buildMessageWidget(companion, message);

        widget->setParent(chatHistoryWidget_.get());

        // if (!widget)
        //     return;

        // auto lambda = [=]()
        // {
        //     // companion->setMappedMessageWidget(message, widget);
        //     getManager()->setMappedMessageWidget(companion, message, widget);
        // };

        // runInDetachedThread(lambda);

        // if (widgetGroup)
        //     widget->setBase(widgetGroup);

        chatHistoryLayout_->addWidget(widget.get());

        if (isAntecedent)
            sortChatHistory(false);
    }

    // widget group action
    // if (widgetGroup)
    //     widgetGroup->messageAdded();

    scrollDownChatHistory();
}

void CentralPanelWidget::scrollDownChatHistory()
{
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    auto value = chatHistoryScrollArea_->verticalScrollBar()->maximum();
    chatHistoryScrollArea_->verticalScrollBar()->setValue(value);
}

void CentralPanelWidget::clearChatHistory()
{
    auto children = chatHistoryWidget_->children();

    for (auto &child : children) {
        // MessageWidgetPtr messageWidget = dynamic_cast<MessageWidgetPtr>(child);
        auto messageWidget = qobject_cast<MessageWidget *>(child);

        // if (messageWidget) {
        //     messageWidget->hide();
        //     delete messageWidget;
        // }
        if (messageWidget)
            messageWidget->hide();
    }
}

void CentralPanelWidget::sortChatHistory(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    auto list = chatHistoryWidget_->children();

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
        // auto message = companion_->getMappedMessageByMessageWidget(false, cast);
        auto message = cast->message();

        coutArgsWithSpaceSeparator("message:", message.get());

        if (message)
            coutArgsWithSpaceSeparator("element message:", message->text());

        chatHistoryLayout_->removeWidget(cast);
        chatHistoryLayout_->addWidget(cast);
    }
}

bool CentralPanelWidget::eventFilter(QObject *object, QEvent *event)
{
    auto result = QWidget::eventFilter(object, event);

    if (object == chatHistoryScrollArea_.get()) {
        auto verticalScrollBar = chatHistoryScrollArea_->verticalScrollBar();

        if (verticalScrollBar && verticalScrollBar->value() == verticalScrollBar->minimum()) {
            auto eventCast = dynamic_cast<QKeyEvent *>(event);
            auto type = event->type();

            bool condition1 = (type == QEvent::Wheel);

            auto keys = (Qt::Key_Up | Qt::Key_PageUp | Qt::Key_Home);
            bool condition21 = (eventCast && (eventCast->key()  &keys));
            bool condition2 = (type == QEvent::KeyPress && condition21);

            if (condition1 || condition2) {
                logTemplateInfo(
                    "scroll bar minimum event type {}", std::to_string(event->type()));

                getManager()->addEarlyMessages(companion_);
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
    getGraphicManager()->sendFile(companion_);
}

RightPanelWidget::RightPanelWidget(QWidgetPtr parent)
{
    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_.get());

    appLogWidget_ = std::make_unique<QPlainTextEdit>();
    // appLogWidget_->setStyleSheet("border-left: 1px solid black");
    appLogWidget_->setReadOnly(true);
    appLogWidget_->setPlainText("");

    appLogWidgetPalette_ = std::make_unique<QPalette>();
    appLogWidgetPalette_->setColor(QPalette::Base, QColor(getWidgetColor(Widget::APP_LOG)));
    appLogWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidget_->setAutoFillBackground(true);
    appLogWidget_->setPalette(*appLogWidgetPalette_);

    layout_->addWidget(appLogWidget_.get());

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
    appLogWidget_->setParent(this);

    connect(
        this, SIGNAL(addTextToAppLogWidgetSignal(const QString &)),
        this, SLOT(addTextToAppLogWidgetSlot(const QString &)),
        Qt::QueuedConnection);

    appLogWidget_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        appLogWidget_.get(), &QWidget::customContextMenuRequested,
        this, &RightPanelWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void RightPanelWidget::addTextToAppLogWidget(const QString &text)
{
    emit addTextToAppLogWidgetSignal(text);
}

void RightPanelWidget::clearLogAction()
{
    appLogWidget_->clear();
}

void RightPanelWidget::addTextToAppLogWidgetSlot(const QString &text)
{
    appLogWidget_->appendPlainText(text);

    // QApplication::processEvents();

    appLogWidget_->ensureCursorVisible();
}

void RightPanelWidget::customMenuRequestedSlot(const QPoint &position)
{
    auto menu = std::make_shared<QMenu>(this);
    auto clearLogAction = std::make_shared<QAction>("Clear log", this);
    menu->addAction(clearLogAction.get());

    connect(
        clearLogAction.get(), &QAction::triggered,
        this, &RightPanelWidget::clearLogAction, Qt::QueuedConnection);

    menu->popup(mapToGlobal(position));
}

MainWidget::MainWidget() : layout_(nullptr) {}

void MainWidget::set()
{
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
    // splitter_-addWidget(leftContainerWidget_);
    splitter_->addWidget(central_.get());
    splitter_->addWidget(right_.get());
    layout_->addWidget(splitter_.get());

    // auto graphicManager = getGraphicManager();
    stubWidgets_ = std::make_shared<StubWidgetGroup>();

    // graphicManager->setParentsForStubs(left_, central_);
    stubWidgets_->setParents(left_, central_);

    // graphicManager->setStubWidgets();
    addWidgetToCompanionPanel(stubWidgets_->socketInfoBase());
    addWidgetToContainerAndSetParentTo(MainWindowContainerPosition::LEFT, stubWidgets_->leftPanel());
    addWidgetToContainerAndSetParentTo(MainWindowContainerPosition::CENTRAL, stubWidgets_->centralPanel());
    addWidgetToContainerAndSetParentTo(MainWindowContainerPosition::RIGHT, stubWidgets_->rightPanel());
    stubWidgets_->set();

    // graphicManager->showCentralPanelStub();
    stubWidgets_->showCentralPanel();

    if (!addWidgetToContainerAndSetParentTo(MainWindowContainerPosition::LEFT, showHideWidget_))
        logArgsError("addWidgetToContainerAndSetParentTo error");

    initBlurEffect();
}

std::shared_ptr<QHBoxLayout> MainWidget::layout() const
{
    return layout_;
}

bool MainWidget::addWidgetToContainerAndSetParentTo(
    MainWindowContainerPosition position, QWidgetPtr widget)
{
    switch (position) {
    case MainWindowContainerPosition::LEFT:
        left_->addWidgetToLayoutAndSetParentTo(widget);

    case MainWindowContainerPosition::CENTRAL:
        central_->addWidgetToLayoutAndSetParentTo(widget);

    case MainWindowContainerPosition::RIGHT:
        right_->addWidgetToLayoutAndSetParentTo(widget);

    default:
        return false;
    }

    return true;
}

void MainWidget::addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget)
{
    left_->addWidgetToCompanionPanel(widget);
}

void MainWidget::removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget)
{
    left_->removeWidgetFromCompanionPanel(widget);
}

std::size_t MainWidget::getCompanionPanelChildrenSize()
{
    return left_->getCompanionPanelChildrenSize();
}

void MainWidget::initBlurEffect()
{
    blurEffect_ = std::make_shared<QGraphicsBlurEffect>();
    blurEffect_->setBlurRadius(BLUR_RADIUS);
}

void MainWidget::addTextToAppLogWidget(const QString &text)
{
    right_->addTextToAppLogWidget(text);
}

void MainWidget::hideLeftAndRightPanels()
{
    left_->hide();
    right_->hide();
}

void MainWidget::showLeftAndRightPanels()
{
    left_->show();
    right_->show();
}

void MainWidget::hideCentralPanelStub()
{
    stubWidgets_->hideCentralPanel();
}

void MainWidget::showCentralPanelStub()
{
    stubWidgets_->showCentralPanel();
}

void MainWidget::hideCompanionPanelStub()
{
    stubWidgets_->hideSocketInfoStubWidget();
}

void MainWidget::showStubPanels()
{
    hideLeftAndRightPanels();
    stubWidgets_->setLeftPanelWidth(left_->getLastCompanionPanelChildWidth());
    stubWidgets_->showStubPanels();
}

void MainWidget::hideStubPanels()
{
    showLeftAndRightPanels();
    stubWidgets_->hideStubPanels();
}

void MainWidget::enableWidgetsForShowHide()
{
    // leftPanel_->setEnabled(true);
    // centralContainerWidget_->setEnabled(true);
    // rightContainerWidget_->setEnabled(true);
    left_->setEnabled(true);
    central_->setEnabled(true);
    right_->setEnabled(true);
}

void MainWidget::disableWidgetsForShowHide()
{
    // leftPanel_->setEnabled(false);
    // centralContainerWidget_->setEnabled(false);
    // rightContainerWidget_->setEnabled(false);
    left_->setEnabled(false);
    central_->setEnabled(false);
    right_->setEnabled(false);
}

void MainWidget::setBlurEffect()
{
    setGraphicsEffect(blurEffect_.get());
}
