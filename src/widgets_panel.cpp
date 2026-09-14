#include "widgets_panel.hpp"

#include <string>
#include <thread>

#include <QApplication>
#include <QMenu>
#include <QScrollBar>

#include "companion.hpp"
#include "constants.hpp"
#include "logging.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils_cout.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"

using namespace std::string_literals;

LeftPanelWidget::LeftPanelWidget(std::shared_ptr<QWidget> parent)
{
    if (parent)
        setParent(parent.get());

    layout_ = std::make_unique<QVBoxLayout>();
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_.get());

    palette_ = std::make_unique<QPalette>();
    palette_->setColor(QPalette::Window, QColor(leftPanelBackgroundColor));
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

void LeftPanelWidget::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
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

void LeftPanelWidget::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
{
    // TODO get rid of children mechanism usage
    auto companionPanelChildren =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widget.get());

    if (index == -1) {
        showErrorDialogAndLogError("SocketInfoBaseWidget was not found in companion panel");
    } else if (index == 0) {

    } else if (index > 0) {
        std::shared_ptr<Manager> manager = getManager();
        auto previousWidget = companionPanelChildren.at(index - 1);
        auto previousCompanion = manager->getMappedCompanionBySocketInfoBaseWidget(previousWidget);
        manager->resetSelectedCompanion(previousCompanion);
        companionPanelLayout_->removeWidget(widget.get());
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth()
{
    // TODO get rid of children mechanism usage
    auto companionPanelChildren =
        companionPanel_->findChildren<SocketInfoBaseWidget *>(Qt::FindDirectChildrenOnly);

    if (companionPanelChildren.size() == 0)
        return -1;
    else
        return companionPanelChildren.at(companionPanelChildren.size() - 1)->width();
}

CentralPanelWidget::CentralPanelWidget(std::shared_ptr<QWidget> parent, const std::string &name)
    : mutex_()
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
    companionNameLabelPalette_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
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
    buttonPanelPalette_->setColor(QPalette::Window, QColor(buttonPanelBackGroundColor));
    chatHistoryWidget_->setPalette(*buttonPanelPalette_);
    sendFileButton_ = std::make_unique<QPushButton>("Send file");
    buttonPanelLayout_->addWidget(sendFileButton_.get());

    layout_->addWidget(buttonPanelWidget_.get());

    textEdit_ = std::make_unique<TextEditWidget>();
    layout_->addWidget(textEdit_.get());

    // splitter_ = new QSplitter;
    // layout_->addWidget(splitter_);
    // splitter_->addWidget(borderWidget_);
    // splitter_->addWidget(textEdit_);
}

void CentralPanelWidget::set(std::shared_ptr<Companion> companion)
{
    companion_ = companion;

    connect(
        textEdit_.get(), &TextEditWidget::send,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        sendFileButton_.get(), &QPushButton::clicked,
        this, &CentralPanelWidget::sendFileSlot, Qt::QueuedConnection);

    chatHistoryScrollArea_->installEventFilter(this);
}

void CentralPanelWidget::addMessageWidgetToChatHistory(
    std::shared_ptr<WidgetGroup> widgetGroup, std::shared_ptr<Companion> companion,
    std::shared_ptr<Message> message, std::shared_ptr<MessageState> state)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        std::shared_ptr<MessageWidget> widget = nullptr;

        switch (message->getType()) {
        case MessageType::TEXT:
            widget = std::make_shared<TextMessageWidget>(
                chatHistoryWidget_, companion, state, message);

            break;

        case MessageType::FILE:
            widget = std::make_shared<FileMessageWidget>(
                chatHistoryWidget_, companion, state, message);

            break;
        }

        auto lambda = [=]()
        {
            companion->setMappedMessageWidget(message, widget);
        };

        std::thread(lambda).detach();

        if (widgetGroup)
            widget->setBase(widgetGroup);

        chatHistoryLayout_->addWidget(widget.get());

        if (state->isAntecedent())
            sortChatHistoryElements(false);
    }

    // widget group action
    if (widgetGroup)
        widgetGroup->messageAdded();

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
        // std::shared_ptr<MessageWidget> messageWidget = dynamic_cast<std::shared_ptr<MessageWidget>>(child);
        auto messageWidget = qobject_cast<MessageWidget *>(child);

        // if (messageWidget) {
        //     messageWidget->hide();
        //     delete messageWidget;
        // }
        if (messageWidget)
            messageWidget->hide();
    }
}

void CentralPanelWidget::sortChatHistoryElements(bool lock)
{
    if (lock)
        std::lock_guard<std::mutex> lockObject(mutex_);

    auto list = chatHistoryWidget_->children();

    auto lambda = [&](const auto &item)
    {
        auto cast = qobject_cast<MessageWidget *>(item);
        auto message = companion_->getMappedMessageByMessageWidget(false, cast);

        return (message) ? message->getTime() : ""s;
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
        auto elementCast = qobject_cast<MessageWidget *>(element);
        auto message = companion_->getMappedMessageByMessageWidget(false, elementCast);

        coutArgsWithSpaceSeparator("message:", message);

        if (message)
            coutArgsWithSpaceSeparator("element message:", message->getText());

        chatHistoryLayout_->removeWidget(elementCast);
        chatHistoryLayout_->addWidget(elementCast);
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
                logArgsWithTemplate(
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

RightPanelWidget::RightPanelWidget(std::shared_ptr<QWidget> parent)
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
    appLogWidgetPalette_->setColor(QPalette::Base, QColor(appLogBackgroundColor));
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
