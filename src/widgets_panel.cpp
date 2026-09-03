#include "widgets_panel.hpp"

LeftPanelWidget::LeftPanelWidget(std::shared_ptr<QWidget> parent) {
    if(parent) {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);

    setLayout(layoutPtr_);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColor(leftPanelBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    companionPanelPtr_ = new QWidget;
    // leftPanelPtr_->setStyleSheet("border-right: 1px solid black");
    companionPanelPtr_->resize(2000, 1000);  // TODO ???
    companionPanelLayoutPtr_ = new QVBoxLayout;
    companionPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    companionPanelLayoutPtr_->setSpacing(0);
    companionPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    companionPanelPtr_->setLayout(companionPanelLayoutPtr_);

    layoutPtr_->addWidget(companionPanelPtr_);

    companionPanelPtr_->resize(4000, 1000);  // TODO ???

    spacerPtr_ = new QSpacerItem(
        0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    layoutPtr_->addSpacerItem(spacerPtr_);
}

LeftPanelWidget::~LeftPanelWidget() {
    delete this->layoutPtr_;
    delete this->palettePtr_;
    delete this->companionPanelPtr_;
    delete this->companionPanelLayoutPtr_;
    delete this->spacerPtr_;
}

void LeftPanelWidget::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->companionPanelLayoutPtr_->addWidget(widget);
}

std::size_t LeftPanelWidget::getCompanionPanelChildrenSize() {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanelPtr_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
            Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void LeftPanelWidget::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widgetPtr) {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanelPtr_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
            Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widgetPtr);

    if(index == -1) {
        showErrorDialogAndLogError(
            "SocketInfoBaseWidget was not found in companion panel");
    }
    else if(index == 0) {

    }
    else if(index > 0) {
        std::shared_ptr<Manager> managerPtr = getManagerPtr();
        auto previousWidget = companionPanelChildren.at(index - 1);

        auto previousCompanionPtr =
            managerPtr->getMappedCompanionBySocketInfoBaseWidget(previousWidget);

        managerPtr->resetSelectedCompanion(previousCompanionPtr);
        this->companionPanelLayoutPtr_->removeWidget(widgetPtr);
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth() {
    QList<std::shared_ptr<SocketInfoBaseWidget>> companionPanelChildren =
        this->companionPanelPtr_->findChildren<std::shared_ptr<SocketInfoBaseWidget>>(
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
    chatHistoryScrollAreaPtr_ = nullptr;
    chatHistoryWidgetPalettePtr_ = nullptr;

    if(parent) {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    companionNameLabelPtr_ = new QLabel(getQString(name));
    // companionNameLabelPtr_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalettePtr_ = new QPalette;
    companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
    companionNameLabelPtr_->setAutoFillBackground(true);
    companionNameLabelPtr_->setPalette(*companionNameLabelPalettePtr_);

    layoutPtr_->addWidget(companionNameLabelPtr_);

    chatHistoryWidgetPtr_ = new QWidget;

    chatHistoryLayoutPtr_ = new QVBoxLayout;
    chatHistoryLayoutPtr_->setSpacing(0);
    chatHistoryLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    chatHistoryLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMaximumSize);
    chatHistoryWidgetPtr_->setLayout(chatHistoryLayoutPtr_);

    if(name.size() != 0) {
        chatHistoryScrollAreaPtr_ = new QScrollArea;
        // chatHistoryScrollAreaPtr_ = new ScrollArea;
        chatHistoryScrollAreaPtr_->setWidgetResizable(true);
        chatHistoryScrollAreaPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        chatHistoryScrollAreaPtr_->setWidget(chatHistoryWidgetPtr_);
        chatHistoryWidgetPalettePtr_ = new QPalette;
        chatHistoryWidgetPalettePtr_->setColor(QPalette::Window, QColorConstants::Gray);
        chatHistoryWidgetPtr_->setPalette(*chatHistoryWidgetPalettePtr_);

        layoutPtr_->addWidget(chatHistoryScrollAreaPtr_);
    }

    buttonPanelWidgetPtr_ = new QWidget;
    buttonPanelLayoutPtr_ = new QHBoxLayout;
    buttonPanelLayoutPtr_->setSpacing(10);
    buttonPanelLayoutPtr_->setContentsMargins(10, 10, 10, 10);
    buttonPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    buttonPanelWidgetPtr_->setLayout(buttonPanelLayoutPtr_);
    buttonPanelPalettePtr_ = new QPalette;
    buttonPanelPalettePtr_->setColor(QPalette::Window, QColor(buttonPanelBackGroundColor));
    chatHistoryWidgetPtr_->setPalette(*buttonPanelPalettePtr_);
    sendFileButtonPtr_ = new QPushButton("Send file");
    buttonPanelLayoutPtr_->addWidget(sendFileButtonPtr_);

    layoutPtr_->addWidget(buttonPanelWidgetPtr_);

    textEditPtr_ = new TextEditWidget;
    layoutPtr_->addWidget(textEditPtr_);

    // splitterPtr_ = new QSplitter;
    // layoutPtr_->addWidget(splitterPtr_);
    // splitterPtr_->addWidget(borderWidgetPtr_);
    // splitterPtr_->addWidget(textEditPtr_);
}

CentralPanelWidget::~CentralPanelWidget() {
    delete this->layoutPtr_;
    delete this->companionNameLabelPtr_;
    delete this->companionNameLabelPalettePtr_;
    delete this->chatHistoryWidgetPtr_;
    delete this->chatHistoryScrollAreaPtr_;
    delete this->chatHistoryWidgetPalettePtr_;
    delete this->chatHistoryLayoutPtr_;
    delete this->textEditPtr_;
    delete this->textEditPalettePtr_;
    delete this->buttonPanelWidgetPtr_;
    delete this->buttonPanelLayoutPtr_;
    delete this->buttonPanelPalettePtr_;
    delete this->sendFileButtonPtr_;

    // TODO delete message widgets
}

void CentralPanelWidget::set(std::shared_ptr<Companion> companionPtr) {
    this->companionPtr_ = companionPtr;

    connect(
        this->textEditPtr_, &TextEditWidget::send,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        this->sendFileButtonPtr_, &QPushButton::clicked,
        this, &CentralPanelWidget::sendFileSlot, Qt::QueuedConnection);

    this->chatHistoryScrollAreaPtr_->installEventFilter(this);
}

void CentralPanelWidget::addMessageWidgetToChatHistory(
    std::shared_ptr<WidgetGroup> widgetGroupPtr, std::shared_ptr<Companion> companionPtr,
    std::shared_ptr<Message> messagePtr, std::shared_ptr<MessageState> messageStatePtr) {
    {
        std::lock_guard<std::mutex> lock(this->chatHistoryMutex_);

        std::shared_ptr<MessageWidget> widgetPtr = nullptr;

        switch(messagePtr->getType()) {
        case MessageType::TEXT:
            widgetPtr = new TextMessageWidget(
                this->chatHistoryWidgetPtr_, companionPtr, messageStatePtr, messagePtr);

            break;

        case MessageType::FILE:
            widgetPtr = new FileMessageWidget(
                this->chatHistoryWidgetPtr_, companionPtr, messageStatePtr, messagePtr);

            break;
        }

        std::thread(
            [=]() {
                companionPtr->setMappedMessageWidget(messagePtr, widgetPtr);
            }
            ).detach();

        if(widgetGroupPtr) {
            widgetPtr->setBase(widgetGroupPtr);
        }

        this->chatHistoryLayoutPtr_->addWidget(widgetPtr);

        if(messageStatePtr->getIsAntecedent()) {
            this->sortChatHistoryElements(false);
        }
    }

    // widget group action
    if(widgetGroupPtr) {
        const_cast<std::shared_ptr<WidgetGroup>>(widgetGroupPtr)->messageAdded();
    }

    this->scrollDownChatHistory();
}

void CentralPanelWidget::scrollDownChatHistory() {
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    this->chatHistoryScrollAreaPtr_->verticalScrollBar()->setValue(
        this->chatHistoryScrollAreaPtr_->verticalScrollBar()->maximum());
}

void CentralPanelWidget::clearChatHistory() {
    auto children = this->chatHistoryWidgetPtr_->children();

    for(auto& child : children) {
        std::shared_ptr<MessageWidget> messageWidgetPtr = dynamic_cast<std::shared_ptr<MessageWidget>>(child);

        if(messageWidgetPtr) {
            messageWidgetPtr->hide();
            delete messageWidgetPtr;
        }
    }
}

void CentralPanelWidget::sortChatHistoryElements(bool lock) {
    if(lock)
        std::lock_guard<std::mutex> lockObject(this->chatHistoryMutex_);

    auto list = this->chatHistoryWidgetPtr_->children();

    auto lambda = [&](auto item) {
        std::shared_ptr<Message> messagePtr =
            companionPtr_->getMappedMessagePtrByMessageWidgetPtr(
                false, dynamic_cast<std::shared_ptr<MessageWidget>>(item));

        return (messagePtr) ? messagePtr->getTime() : std::string("");
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

        std::shared_ptr<Message> messagePtr =
            companionPtr_->getMappedMessagePtrByMessageWidgetPtr(false, elementCast);

        coutArgsWithSpaceSeparator("messagePtr:", messagePtr);

        if(messagePtr) {
            coutArgsWithSpaceSeparator("element message:", messagePtr->getText());
        }

        this->chatHistoryLayoutPtr_->removeWidget(elementCast);
        this->chatHistoryLayoutPtr_->addWidget(elementCast);
    }
}

bool CentralPanelWidget::eventFilter(QObject *object, QEvent *event) {
    auto result = QWidget::eventFilter(objectPtr, eventPtr);

    if(objectPtr == this->chatHistoryScrollAreaPtr_) {
        auto verticalScrollBarPtr =
            this->chatHistoryScrollAreaPtr_->verticalScrollBar();

        if(verticalScrollBarPtr &&
            verticalScrollBarPtr->value() == verticalScrollBarPtr->minimum()) {
            std::shared_ptr<QKeyEvent> eventCastPtr = dynamic_cast<std::shared_ptr<QKeyEvent>>(eventPtr);

            if(eventPtr->type() == QEvent::Wheel ||
                (eventPtr->type() == QEvent::KeyPress &&
                 eventCastPtr &&
                 (eventCastPtr->key() &
                  (Qt::Key_Up | Qt::Key_PageUp | Qt::Key_Home)))) {
                logArgsWithTemplate(
                    "scroll bar minimum event type {}", std::to_string(eventPtr->type()));

                getManagerPtr()->addEarlyMessages(this->companionPtr_);
            }
        }
    }

    return result;
}

void CentralPanelWidget::sendMessage(const QString& text) {
    if(!text.isEmpty()) {
        getGraphicManagerPtr()->sendMessage(
            MessageType::TEXT, this->companionPtr_, text.toStdString());
    }
}

void CentralPanelWidget::sendFileSlot() {
    getGraphicManagerPtr()->sendFile(this->companionPtr_);
}

void CentralPanelWidget::saveFileSlot() {
    getGraphicManagerPtr()->sendFile(this->companionPtr_);
}

RightPanelWidget::RightPanelWidget(std::shared_ptr<QWidget> parent) {
    if(parent) {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    appLogWidgetPtr_ = new QPlainTextEdit;
    // appLogWidgetPtr_->setStyleSheet("border-left: 1px solid black");
    appLogWidgetPtr_->setReadOnly(true);
    appLogWidgetPtr_->setPlainText("");

    appLogWidgetPalettePtr_ = new QPalette;
    appLogWidgetPalettePtr_->setColor(QPalette::Base, QColor(appLogBackgroundColor));
    appLogWidgetPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidgetPtr_->setAutoFillBackground(true);
    appLogWidgetPtr_->setPalette(*appLogWidgetPalettePtr_);

    layoutPtr_->addWidget(appLogWidgetPtr_);

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);
}

RightPanelWidget::~RightPanelWidget() {
    delete this->layoutPtr_;
    delete this->appLogWidgetPtr_;
    delete this->appLogWidgetPalettePtr_;
}

void RightPanelWidget::set() {
    this->appLogWidgetPtr_->setParent(this);

    connect(
        this, SIGNAL(addTextToAppLogWidgetSignal(const QString&)),
        this, SLOT(addTextToAppLogWidgetSlot(const QString&)),
        Qt::QueuedConnection);

    this->appLogWidgetPtr_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this->appLogWidgetPtr_, &QWidget::customContextMenuRequested,
        this, &RightPanelWidget::customMenuRequestedSlot, Qt::QueuedConnection);
}

void RightPanelWidget::addTextToAppLogWidget(const QString& text) {
    emit this->addTextToAppLogWidgetSignal(text);
}

void RightPanelWidget::clearLogAction() {
    this->appLogWidgetPtr_->clear();
}

void RightPanelWidget::addTextToAppLogWidgetSlot(const QString& text) {
    this->appLogWidgetPtr_->appendPlainText(text);

    // QApplication::processEvents();

    this->appLogWidgetPtr_->ensureCursorVisible();
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
