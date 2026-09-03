#include "widgets_panel.hpp"

LeftPanelWidget::LeftPanelWidget(std::shared_ptr<QWidget> parent) {
    if(parent) {
        setParent(parent);
    }

    layout_ = new QVBoxLayout;
    layout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);

    setLayout(layout_);

    palette_ = new QPalette;
    palette_->setColor(QPalette::Window, QColor(leftPanelBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palette_);

    companionPanel_ = new QWidget;
    // leftPanel_->setStyleSheet("border-right: 1px solid black");
    companionPanel_->resize(2000, 1000);  // TODO ???
    companionPanelLayout_ = new QVBoxLayout;
    companionPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    companionPanelLayout_->setSpacing(0);
    companionPanelLayout_->setContentsMargins(0, 0, 0, 0);
    companionPanel_->setLayout(companionPanelLayout_);

    layout_->addWidget(companionPanel_);

    companionPanel_->resize(4000, 1000);  // TODO ???

    spacer_ = new QSpacerItem(
        0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout_->addSpacerItem(spacer_);
}

void LeftPanelWidget::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->companionPanelLayout_->addWidget(widget);
}

std::size_t LeftPanelWidget::getCompanionPanelChildrenSize() {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanel_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
            Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void LeftPanelWidget::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanel_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
            Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widget);

    if(index == -1) {
        showErrorDialogAndLogError(
            "SocketInfoBaseWidget was not found in companion panel");
    }
    else if(index == 0) {

    }
    else if(index > 0) {
        std::shared_ptr<Manager> manager = getManager();
        auto previousWidget = companionPanelChildren.at(index - 1);

        auto previousCompanion =
            manager->getMappedCompanionBySocketInfoBaseWidget(previousWidget);

        manager->resetSelectedCompanion(previousCompanion);
        this->companionPanelLayout_->removeWidget(widget);
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth() {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanel_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
            Qt::FindDirectChildrenOnly);

    if(companionPanelChildren.size() == 0) {
        return -1;
    }
    else {
        return companionPanelChildren.at(companionPanelChildren.size() - 1)->width();
    }
}

CentralPanelWidget::CentralPanelWidget(std::shared_ptr<QWidget> parent, const std::string& name) :
    chatHistoryMutex_(std::mutex()) {
    chatHistoryScrollArea_ = nullptr;
    chatHistoryWidgetPalette_ = nullptr;

    if(parent) {
        setParent(parent);
    }

    layout_ = new QVBoxLayout;
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);

    companionNameLabel_ = new QLabel(getQString(name));
    // companionNameLabel_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalette_ = new QPalette;
    companionNameLabelPalette_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
    companionNameLabel_->setAutoFillBackground(true);
    companionNameLabel_->setPalette(*companionNameLabelPalette_);

    layout_->addWidget(companionNameLabel_);

    chatHistoryWidget_ = new QWidget;

    chatHistoryLayout_ = new QVBoxLayout;
    chatHistoryLayout_->setSpacing(0);
    chatHistoryLayout_->setContentsMargins(0, 0, 0, 0);
    chatHistoryLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    chatHistoryLayout_->setSizeConstraint(QLayout::SetMaximumSize);
    chatHistoryWidget_->setLayout(chatHistoryLayout_);

    if(name.size() != 0) {
        chatHistoryScrollArea_ = new QScrollArea;
        // chatHistoryScrollArea_ = new ScrollArea;
        chatHistoryScrollArea_->setWidgetResizable(true);
        chatHistoryScrollArea_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        chatHistoryScrollArea_->setWidget(chatHistoryWidget_);
        chatHistoryWidgetPalette_ = new QPalette;
        chatHistoryWidgetPalette_->setColor(QPalette::Window, QColorConstants::Gray);
        chatHistoryWidget_->setPalette(*chatHistoryWidgetPalette_);

        layout_->addWidget(chatHistoryScrollArea_);
    }

    buttonPanelWidget_ = new QWidget;
    buttonPanelLayout_ = new QHBoxLayout;
    buttonPanelLayout_->setSpacing(10);
    buttonPanelLayout_->setContentsMargins(10, 10, 10, 10);
    buttonPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buttonPanelWidget_->setLayout(buttonPanelLayout_);
    buttonPanelPalette_ = new QPalette;
    buttonPanelPalette_->setColor(QPalette::Window, QColor(buttonPanelBackGroundColor));
    chatHistoryWidget_->setPalette(*buttonPanelPalette_);
    sendFileButton_ = new QPushButton("Send file");
    buttonPanelLayout_->addWidget(sendFileButton_);

    layout_->addWidget(buttonPanelWidget_);

    textEdit_ = new TextEditWidget;
    layout_->addWidget(textEdit_);

    // splitter_ = new QSplitter;
    // layout_->addWidget(splitter_);
    // splitter_->addWidget(borderWidget_);
    // splitter_->addWidget(textEdit_);
}

void CentralPanelWidget::set(std::shared_ptr<Companion> companion) {
    this->companion_ = companion;

    connect(
        this->textEdit_, &TextEditWidget::send,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        this->sendFileButton_, &QPushButton::clicked,
        this, &CentralPanelWidget::sendFileSlot, Qt::QueuedConnection);

    this->chatHistoryScrollArea_->installEventFilter(this);
}

void CentralPanelWidget::addMessageWidgetToChatHistory(
    std::shared_ptr<WidgetGroup> widgetGroup, std::shared_ptr<Companion> companion,
    std::shared_ptr<Message> message, std::shared_ptr<MessageState> messageState) {
    {
        std::lock_guard<std::mutex> lock(this->chatHistoryMutex_);

        std::shared_ptr<MessageWidget> widget = nullptr;

        switch(message->getType()) {
        case MessageType::TEXT:
            widget = new TextMessageWidget(
                this->chatHistoryWidget_, companion, messageState, message);

            break;

        case MessageType::FILE:
            widget = new FileMessageWidget(
                this->chatHistoryWidget_, companion, messageState, message);

            break;
        }

        std::thread(
            [=]() {
                companion->setMappedMessageWidget(message, widget);
            }
            ).detach();

        if(widgetGroup) {
            widget->setBase(widgetGroup);
        }

        this->chatHistoryLayout_->addWidget(widget);

        if(messageState->getIsAntecedent()) {
            this->sortChatHistoryElements(false);
        }
    }

    // widget group action
    if(widgetGroup) {
        const_cast<std::shared_ptr<WidgetGroup>>(widgetGroup)->messageAdded();
    }

    this->scrollDownChatHistory();
}

void CentralPanelWidget::scrollDownChatHistory() {
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    this->chatHistoryScrollArea_->verticalScrollBar()->setValue(
        this->chatHistoryScrollArea_->verticalScrollBar()->maximum());
}

void CentralPanelWidget::clearChatHistory() {
    auto children = this->chatHistoryWidget_->children();

    for(auto& child : children) {
        // std::shared_ptr<MessageWidget> messageWidget = dynamic_cast<std::shared_ptr<MessageWidget>>(child);
        auto messageWidget = dynamic_pointer_cast<MessageWidget>(child);

        // if(messageWidget) {
        //     messageWidget->hide();
        //     delete messageWidget;
        // }
        if(messageWidget)
            messageWidget->hide();
    }
}

void CentralPanelWidget::sortChatHistoryElements(bool lock) {
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->chatHistoryMutex_);

    auto list = this->chatHistoryWidget_->children();

    auto lambda = [&](auto item) {
        std::shared_ptr<Message> message =
            companion_->getMappedMessageByMessageWidget(
                false, dynamic_cast<std::shared_ptr<MessageWidget>>(item));

        return (message) ? message->getTime() : std::string("");
    };

    std::sort(
        list.begin(),
        list.end(),
        [&](auto element1, auto element2) {
            auto res1 = lambda(element1);
            auto res2 = lambda(element2);
            bool res = (res1 < res2);

            coutArgsWithSpaceSeparator("res1:", res1, "res2:", res2, "res:", res);

            return res;
        });

    coutArgsWithSpaceSeparator("AFTER SORTING");

    for(auto& element : list) {
        auto elementCast = dynamic_cast<std::shared_ptr<MessageWidget>>(element);

        std::shared_ptr<Message> message =
            companion_->getMappedMessageByMessageWidget(false, elementCast);

        coutArgsWithSpaceSeparator("message:", message);

        if(message) {
            coutArgsWithSpaceSeparator("element message:", message->getText());
        }

        this->chatHistoryLayout_->removeWidget(elementCast);
        this->chatHistoryLayout_->addWidget(elementCast);
    }
}

bool CentralPanelWidget::eventFilter(QObject *object, QEvent *event) {
    auto result = QWidget::eventFilter(object, event);

    if(object == this->chatHistoryScrollArea_) {
        auto verticalScrollBar =
            this->chatHistoryScrollArea_->verticalScrollBar();

        if(verticalScrollBar &&
            verticalScrollBar->value() == verticalScrollBar->minimum()) {
            std::shared_ptr<QKeyEvent> eventCast = dynamic_cast<std::shared_ptr<QKeyEvent>>(event);

            if(event->type() == QEvent::Wheel ||
                (event->type() == QEvent::KeyPress &&
                 eventCast &&
                 (eventCast->key() &
                  (Qt::Key_Up | Qt::Key_PageUp | Qt::Key_Home)))) {
                logArgsWithTemplate(
                    "scroll bar minimum event type {}", std::to_string(event->type()));

                getManager()->addEarlyMessages(this->companion_);
            }
        }
    }

    return result;
}

void CentralPanelWidget::sendMessage(const QString& text) {
    if(!text.isEmpty()) {
        getGraphicManager()->sendMessage(
            MessageType::TEXT, this->companion_, text.toStdString());
    }
}

void CentralPanelWidget::sendFileSlot() {
    getGraphicManager()->sendFile(this->companion_);
}

void CentralPanelWidget::saveFileSlot() {
    getGraphicManager()->sendFile(this->companion_);
}

RightPanelWidget::RightPanelWidget(std::shared_ptr<QWidget> parent) {
    if(parent) {
        setParent(parent);
    }

    layout_ = new QVBoxLayout;
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);

    appLogWidget_ = new QPlainTextEdit;
    // appLogWidget_->setStyleSheet("border-left: 1px solid black");
    appLogWidget_->setReadOnly(true);
    appLogWidget_->setPlainText("");

    appLogWidgetPalette_ = new QPalette;
    appLogWidgetPalette_->setColor(QPalette::Base, QColor(appLogBackgroundColor));
    appLogWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidget_->setAutoFillBackground(true);
    appLogWidget_->setPalette(*appLogWidgetPalette_);

    layout_->addWidget(appLogWidget_);

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);
}

void RightPanelWidget::set() {
    this->appLogWidget_->setParent(this);

    connect(
        this, SIGNAL(addTextToAppLogWidgetSignal(const QString&)),
        this, SLOT(addTextToAppLogWidgetSlot(const QString&)),
        Qt::QueuedConnection);

    this->appLogWidget_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this->appLogWidget_, &QWidget::customContextMenuRequested,
        this, &RightPanelWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void RightPanelWidget::addTextToAppLogWidget(const QString& text) {
    emit this->addTextToAppLogWidgetSignal(text);
}

void RightPanelWidget::clearLogAction() {
    this->appLogWidget_->clear();
}

void RightPanelWidget::addTextToAppLogWidgetSlot(const QString& text) {
    this->appLogWidget_->appendPlainText(text);

    // QApplication::processEvents();

    this->appLogWidget_->ensureCursorVisible();
}

void RightPanelWidget::customMenuRequestedSlot(QPoint position) {
    std::shared_ptr<QMenu> menu = new QMenu(this);

    std::shared_ptr<QAction> clearLogAction = new QAction("Clear log", this);
    menu->addAction(clearLogAction);

    connect(
        clearLogAction, &QAction::triggered,
        this, &RightPanelWidget::clearLogAction, Qt::QueuedConnection);

    menu->popup(this->mapToGlobal(position));
}
