#include "widgets.hpp"

QString getInitialConnectButtonLabel()
{
    try
    {
        return connectButtonLabels.empty() ? QString("") : connectButtonLabels.at(0);
    }
    catch(...)
    {
        return QString("_?_");
    }
}

QString getNextConnectButtonLabel(QString& currentLabel)
{
    try
    {
        if(connectButtonLabels.empty())
        {
            return QString("");
        }
        else
        {
            auto currentIterator = std::find(
                connectButtonLabels.begin(),
                connectButtonLabels.end(),
                currentLabel);

            if(currentIterator == connectButtonLabels.end())
            {
                return *connectButtonLabels.begin();
            }

            auto nextIterator = currentIterator + 1;

            if(nextIterator == connectButtonLabels.end())
            {
                nextIterator = connectButtonLabels.begin();
            }

            return *nextIterator;
        }
    }
    catch(...)
    {
        return QString("_?_");
    }
}

TextEditWidget::TextEditWidget()
{
    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Base, QColor(textEditBackgroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

TextEditWidget::~TextEditWidget()
{
    delete this->palettePtr_;
}

void TextEditWidget::keyPressEvent(QKeyEvent* event)
{
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress && event->key() == Qt::Key_Return)
    {
        if(event->modifiers() == Qt::NoModifier)
        {
            this->send(this->toPlainText());
            this->setText("");
        }
        else if(event->modifiers() == Qt::ControlModifier)
        {
            QKeyEvent eventEmulated = QKeyEvent(
                QEvent::KeyPress,
                Qt::Key_Return,
                Qt::NoModifier);

            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}

IndicatorWidget::IndicatorWidget(uint8_t size)
{
    size_ = size;
    setFixedWidth(size_);
    setFixedHeight(size_);

    isOn_ = false;
    onColor_ = QColor(QColorConstants::DarkGreen);
    offColor_ = QColor(QColorConstants::DarkRed);
    meColor_ = QColor(indicatorMeColor);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, offColor_);

    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

IndicatorWidget::IndicatorWidget(const IndicatorWidget* indicator)
{
    size_ = indicator->size_;
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palettePtr_ = indicator->palettePtr_;
}

IndicatorWidget::~IndicatorWidget()
{
    delete this->palettePtr_;
}

void IndicatorWidget::setOn()
{
    this->isOn_ = true;
    this->palettePtr_->setColor(QPalette::Window, onColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setOff()
{
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, offColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::setMe()
{
    this->isOn_ = false;
    this->palettePtr_->setColor(QPalette::Window, meColor_);
    this->setPalette(*this->palettePtr_);
}

void IndicatorWidget::toggle()
{
    (this->isOn_ == true) ? this->setOff() : this->setOn();
}

SocketInfoWidget::SocketInfoWidget()
{
    logArgs("SocketInfoWidget()");
}

SocketInfoWidget::SocketInfoWidget(const SocketInfoWidget& si)
{
    name_ = si.name_;
    ipAddress_ = si.ipAddress_;
    serverPort_ = si.serverPort_;
    clientPort_ = si.clientPort_;

    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string& name, std::string& ipAddress, uint16_t& serverPort, uint16_t& clientPort) :
    name_(QString::fromStdString(name)),
    ipAddress_(QString::fromStdString(ipAddress)),
    serverPort_(serverPort),
    clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(
    std::string&& name, std::string&& ipAddress, uint16_t&& serverPort, uint16_t&& clientPort) :
    name_(QString::fromStdString(name)),
    ipAddress_(QString::fromStdString(ipAddress)),
    serverPort_(serverPort),
    clientPort_(clientPort)
{
    initializeFields();
}

SocketInfoWidget::SocketInfoWidget(Companion* companionPtr) :
    companionPtr_(companionPtr),
    name_(QString::fromStdString(companionPtr->getName())),
    ipAddress_(QString::fromStdString(companionPtr->getSocketInfoPtr()->getIpAddress())),
    serverPort_(companionPtr->getSocketInfoPtr()->getServerPort()),
    clientPort_(companionPtr->getSocketInfoPtr()->getClientPort())
{
    initializeFields();
}

SocketInfoWidget::~SocketInfoWidget()
{
    // cannot set parent
    delete this->palettePtr_;
}

void SocketInfoWidget::print()
{
    logArgs(
        "name:", this->name_, "ipAddress:", this->ipAddress_,
        "serverPort_:", this->serverPort_, "clientPort_:", this->clientPort_);
}

bool SocketInfoWidget::isStub()
{
    return false;
}

void SocketInfoWidget::initializeFields()
{
    selectedColor_ = QColor(QColorConstants::DarkGray);
    unselectedColor_ = QColor(QColorConstants::Gray);
    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, unselectedColor_);
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);
    indicatorPtr_ = new IndicatorWidget(15);
    nameLabelPtr_ = new QLabel(name_);
    ipAddressLabelPtr_ = new QLabel(ipAddress_);

    QString serverPortQString = QString::fromStdString(std::to_string(serverPort_));
    QString clientPortQString = QString::fromStdString(std::to_string(clientPort_));

    serverPortLabelPtr_ = new QLabel(serverPortQString);
    clientPortLabelPtr_ = new QLabel(clientPortQString);
    editButtonPtr_ = new QPushButton("Edit");
    connectButtonPtr_ = new QPushButton(getInitialConnectButtonLabel());

    connect(
        this->editButtonPtr_, &QPushButton::clicked,
        this, &SocketInfoWidget::updateCompanionAction, Qt::QueuedConnection);

    connect(
        this->connectButtonPtr_, &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction, Qt::QueuedConnection);

    if(name_ == "me")  // TODO ???
    {
        indicatorPtr_->setMe();
        editButtonPtr_->hide();
        connectButtonPtr_->hide();
    }

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    std::initializer_list<QWidget*> widgets
    {
        indicatorPtr_, nameLabelPtr_, ipAddressLabelPtr_, serverPortLabelPtr_,
        clientPortLabelPtr_, editButtonPtr_, connectButtonPtr_
    };

    for(auto& widget : widgets)
    {
        layoutPtr_->addWidget(widget);
    }

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(
        this, &QWidget::customContextMenuRequested,
        this, &SocketInfoWidget::customMenuRequestedSlot, Qt::QueuedConnection);

    // connect
}

void SocketInfoWidget::customMenuRequestedSlot(QPoint pos)
{
    QMenu* menu = new QMenu(this);

    QAction* clearHistoryAction = new QAction("Clear chat history", this);
    menu->addAction(clearHistoryAction);

    connect(
        clearHistoryAction, &QAction::triggered,
        this, &SocketInfoWidget::clearHistoryAction, Qt::QueuedConnection);

    QAction* deleteCompanionAction = new QAction("Delete companion", this);
    menu->addAction(deleteCompanionAction);

    connect(
        deleteCompanionAction, &QAction::triggered,
        this, &SocketInfoWidget::deleteCompanionAction, Qt::QueuedConnection);

    menu->popup(this->mapToGlobal(pos));
}

void SocketInfoWidget::updateCompanionAction()
{
    getGraphicManagerPtr()->updateCompanion(this->companionPtr_);
}

void SocketInfoWidget::clearHistoryAction()
{
    getGraphicManagerPtr()->clearCompanionHistory(this->companionPtr_);
}

void SocketInfoWidget::deleteCompanionAction()
{
    getGraphicManagerPtr()->deleteCompanion(this->companionPtr_);
}

void SocketInfoWidget::clientAction()
{
    bool result = false;

    const Companion* companion =
        getManagerPtr()->getMappedCompanionBySocketInfoBaseWidget(this);

    // TODO change to states
    QString currentText = this->connectButtonPtr_->text();

    if(currentText == connectButtonConnectLabel)
    {
        result = const_cast<Companion*>(companion)->connectClient();
        getManagerPtr()->sendUnsentMessages(companion);
    }
    else if(currentText == connectButtonDisconnectLabel)
    {
        result = const_cast<Companion*>(companion)->disconnectClient();
    }

    if(result)
    {
        // change connect button text
        QString nextText = getNextConnectButtonLabel(currentText);
        this->connectButtonPtr_->setText(nextText);

        // change indicator color
        this->indicatorPtr_->toggle();
    }
}

void SocketInfoWidget::changeColor(QColor& color)
{
    this->palettePtr_ = new QPalette;
    this->palettePtr_->setColor(QPalette::Window, color);

    this->setAutoFillBackground(true);
    this->setPalette(*this->palettePtr_);
}

void SocketInfoWidget::select()
{
    this->changeColor(this->selectedColor_);
}

void SocketInfoWidget::unselect()
{
    this->changeColor(this->unselectedColor_);
}

void SocketInfoWidget::update()
{
    this->name_ = QString::fromStdString(this->companionPtr_->getName());
    this->nameLabelPtr_->setText(this->name_);
    this->ipAddress_ = QString::fromStdString(this->companionPtr_->getIpAddress());
    this->ipAddressLabelPtr_->setText(this->ipAddress_);
    this->clientPort_ = this->companionPtr_->getClientPort();
    this->clientPortLabelPtr_->setText(QString::fromStdString(std::to_string(this->clientPort_)));
}

void SocketInfoWidget::mousePressEvent(QMouseEvent* event)
{
    Manager* managerPtr = getManagerPtr();

    auto base = dynamic_cast<SocketInfoBaseWidget*>(this);

    auto newCompanion =
        managerPtr->getMappedCompanionBySocketInfoBaseWidget(base);

    managerPtr->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent* event) {}

SocketInfoStubWidget::SocketInfoStubWidget()
{
    mark_ = socketInfoStubWidget;

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);

    markLabelPtr_ = new QLabel(mark_);
    layoutPtr_->addWidget(markLabelPtr_);
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

MessageIndicatorPanelWidget::MessageIndicatorPanelWidget(
    bool isMessageFromMe, uint8_t isAntecedent)
{
    isMessageFromMe_ = isMessageFromMe;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layoutPtr_ = new QHBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layoutPtr_->setSpacing(5);
    layoutPtr_->setContentsMargins(0, 0, 10, 10);

    setLayout(layoutPtr_);

    if(isMessageFromMe_)
    {
        sentIndicatoPtr_ = new IndicatorWidget(10);
        receivedIndicatoPtr_ = new IndicatorWidget(10);

        newMessageLabelPtr_ = nullptr;

        layoutPtr_->addWidget(sentIndicatoPtr_);
        layoutPtr_->addWidget(receivedIndicatoPtr_);
    }
    else
    {
        sentIndicatoPtr_ = nullptr;
        receivedIndicatoPtr_ = nullptr;

        QString text = (isAntecedent) ? "NEW" : "";

        QString textHtml = QString("<font color=\"%1\"><b>%2</b></font>")
            .arg(receivedMessageColor, text);

        // newMessageEditPtr_ = new QTextEdit;
        newMessageLabelPtr_ = new QLabel(textHtml);
        newMessageLabelPtr_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
        // newMessageEditPtr_->setReadOnly(true);
        // newMessageEditPtr_->insertHtml(textHtml);

        layoutPtr_->addWidget(newMessageLabelPtr_);
    }
}

MessageIndicatorPanelWidget::~MessageIndicatorPanelWidget()
{
    delete this->layoutPtr_;
    delete this->sentIndicatoPtr_;
    delete this->receivedIndicatoPtr_;
    delete this->newMessageLabelPtr_;
}

void MessageIndicatorPanelWidget::setSentIndicatorOn()
{
    this->sentIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::setReceivedIndicatorOn()
{
    this->receivedIndicatoPtr_->setOn();
}

void MessageIndicatorPanelWidget::unsetNewMessageLabel()
{
    if(this->newMessageLabelPtr_)
    {
        this->newMessageLabelPtr_->setText("");
    }
}

MessageWidget::MessageWidget(
    QWidget* parentPtr, uint8_t isAntecedent,
    const std::string& companionName, const Message* messagePtr)
{
    isMessageFromMe_ = messagePtr->isMessageFromMe();

    // set parent
    if(parentPtr)
    {
        setParent(parentPtr);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColorConstants::LightGray);
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);

    setLayout(layoutPtr_);

    auto data = formatMessageHeaderAndBody(companionName, messagePtr);

    headerLabelPtr_ = new QLabel(data.first);
    layoutPtr_->addWidget(headerLabelPtr_);

    messageLabelPtr_ = new QLabel(data.second);
    layoutPtr_->addWidget(messageLabelPtr_);

    indicatorPanelPtr_ = new MessageIndicatorPanelWidget(isMessageFromMe_, isAntecedent);
    layoutPtr_->addWidget(indicatorPanelPtr_);
}

MessageWidget::~MessageWidget()
{
    delete this->palettePtr_;
    delete this->layoutPtr_;
    delete this->headerLabelPtr_;
    delete this->messageLabelPtr_;
    delete this->indicatorPanelPtr_;
}

void MessageWidget::set()
{
    this->indicatorPanelPtr_->setParent(this);
}

void MessageWidget::setMessageAsSent()
{
    this->indicatorPanelPtr_->setSentIndicatorOn();
}

void MessageWidget::setMessageAsReceived()
{
    this->indicatorPanelPtr_->setReceivedIndicatorOn();
}

void MessageWidget::mousePressEvent(QMouseEvent* event)
{
    this->indicatorPanelPtr_->unsetNewMessageLabel();
}

LeftPanelWidget::LeftPanelWidget(QWidget* parent)
{
    if(parent)
    {
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

LeftPanelWidget::~LeftPanelWidget()
{
    delete this->layoutPtr_;
    delete this->palettePtr_;
    delete this->companionPanelPtr_;
    delete this->companionPanelLayoutPtr_;
    delete this->spacerPtr_;
}

void LeftPanelWidget::addWidgetToCompanionPanel(SocketInfoBaseWidget* widget)
{
    this->companionPanelLayoutPtr_->addWidget(widget);
}

size_t LeftPanelWidget::getCompanionPanelChildrenSize()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void LeftPanelWidget::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widgetPtr);

    if(index == -1)
    {
        showErrorDialogAndLogError(nullptr, "SocketInfoBaseWidget was not found in companion panel");
    }
    else if(index == 0)
    {

    }
    else if(index > 0)
    {
        Manager* managerPtr = getManagerPtr();

        auto previousWidget = companionPanelChildren.at(index - 1);

        auto previousCompanionPtr = managerPtr->
                                    getMappedCompanionBySocketInfoBaseWidget(previousWidget);

        managerPtr->resetSelectedCompanion(previousCompanionPtr);

        this->companionPanelLayoutPtr_->removeWidget(widgetPtr);
    }
}

int LeftPanelWidget::getLastCompanionPanelChildWidth()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    if(companionPanelChildren.size() == 0)
    {
        return -1;
    }
    else
    {
        return companionPanelChildren.at(companionPanelChildren.size() - 1)->width();
    }
}

CentralPanelWidget::CentralPanelWidget(QWidget* parent, const std::string& name) :
    chatHistoryMutex_(std::mutex())
{
    if(parent)
    {
        setParent(parent);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    companionNameLabelPtr_ = new QLabel(QString::fromStdString(name));
    // companionNameLabelPtr_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalettePtr_ = new QPalette;
    companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
    companionNameLabelPtr_->setAutoFillBackground(true);
    companionNameLabelPtr_->setPalette(*companionNameLabelPalettePtr_);

    layoutPtr_->addWidget(companionNameLabelPtr_);

    // chatHistoryWidgetPtr_ = new QTextEdit;
    // chatHistoryWidgetPtr_->setTextColor(QColorConstants::Green);
    // chatHistoryWidgetPtr_->setReadOnly(true);
    // chatHistoryWidgetPtr_->setPlainText("");

    chatHistoryWidgetPtr_ = new QWidget;

    chatHistoryLayoutPtr_ = new QVBoxLayout;
    chatHistoryLayoutPtr_->setSpacing(0);
    chatHistoryLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    chatHistoryLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    chatHistoryLayoutPtr_->setSizeConstraint(QLayout::SetMaximumSize);
    chatHistoryWidgetPtr_->setLayout(chatHistoryLayoutPtr_);

    chatHistoryScrollAreaPtr_ = new QScrollArea;
    chatHistoryScrollAreaPtr_->setWidgetResizable(true);
    chatHistoryScrollAreaPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    chatHistoryScrollAreaPtr_->setWidget(chatHistoryWidgetPtr_);

    chatHistoryWidgetPalettePtr_ = new QPalette;
    chatHistoryWidgetPalettePtr_->setColor(QPalette::Window, QColorConstants::Gray);
    chatHistoryWidgetPtr_->setPalette(*chatHistoryWidgetPalettePtr_);

    // layoutPtr_->addWidget(chatHistoryWidgetPtr_);
    layoutPtr_->addWidget(chatHistoryScrollAreaPtr_);

    textEditPtr_ = new TextEditWidget;    
    layoutPtr_->addWidget(textEditPtr_);
}

CentralPanelWidget::~CentralPanelWidget()
{
    delete this->layoutPtr_;
    delete this->companionNameLabelPtr_;
    delete this->companionNameLabelPalettePtr_;
    delete this->chatHistoryWidgetPtr_;
    delete this->chatHistoryScrollAreaPtr_;
    delete this->chatHistoryWidgetPalettePtr_;
    delete this->chatHistoryLayoutPtr_;
    delete this->textEditPtr_;
    delete this->textEditPalettePtr_;

    // TODO delete message widgets
}

void CentralPanelWidget::set(Companion* companionPtr)
{
    this->companionPtr_ = companionPtr;

    connect(
        this->textEditPtr_, &TextEditWidget::send,
        this, &CentralPanelWidget::sendMessage, Qt::QueuedConnection);

    connect(
        this, &CentralPanelWidget::addMessageWidgetToChatHistorySignal,
        this, &CentralPanelWidget::addMessageWidgetToChatHistorySlot,
        Qt::QueuedConnection);
}

void CentralPanelWidget::addMessageWidgetToChatHistory(
    bool isAntecedent, const std::string& companionName, const Message* messagePtr)
{
    {
        std::lock_guard<std::mutex> lock(this->chatHistoryMutex_);

        MessageWidget* widgetPtr = new MessageWidget(
            this->chatHistoryWidgetPtr_, isAntecedent, companionName, messagePtr);

        getGraphicManagerPtr()->addToMessageMapping(messagePtr, widgetPtr);

        widgetPtr->set();

        this->chatHistoryLayoutPtr_->addWidget(widgetPtr);

        if(isAntecedent)
        {
            this->sortChatHistoryElements(false);
        }
    }

    // logArgsWithCustomMark(
    //     "this->chatHistoryWidgetPtr_->children().size():",
    //     this->chatHistoryWidgetPtr_->children().size());

    this->scrollDownChatHistory();    
}

void CentralPanelWidget::addMessageWidgetToChatHistoryFromThread(
    bool isAntecedent, const std::string& companionName, const Message* messagePtr)
{
    QString companionNameQString = QString::fromStdString(companionName);

    emit this->addMessageWidgetToChatHistorySignal(
        isAntecedent, companionNameQString, messagePtr);
}

void CentralPanelWidget::scrollDownChatHistory()
{
    // double call workaround to scroll down fully
    // TODO find out
    QApplication::processEvents();
    QApplication::processEvents();

    this->chatHistoryScrollAreaPtr_->verticalScrollBar()->setValue(
        this->chatHistoryScrollAreaPtr_->verticalScrollBar()->maximum());
}

void CentralPanelWidget::addMessageWidgetToChatHistorySlot(
    bool isAntecedent, const QString& companionName, const Message* messagePtr)
{
    this->addMessageWidgetToChatHistory(
        isAntecedent, companionName.toStdString(), messagePtr);
}

void CentralPanelWidget::clearChatHistory()
{
    auto children = this->chatHistoryWidgetPtr_->children();

    for(auto& child : children)
    {
        MessageWidget* messageWidgetPtr = dynamic_cast<MessageWidget*>(child);

        if(messageWidgetPtr)
        {
            messageWidgetPtr->hide();
            delete messageWidgetPtr;
        }
    }
}

void CentralPanelWidget::sortChatHistoryElements(bool lock)
{
    if(lock)
        std::lock_guard<std::mutex> lock(this->chatHistoryMutex_);

    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();
    auto list = this->chatHistoryWidgetPtr_->children();

    auto lambda = [&](auto item)
    {
        auto result = graphicManagerPtr->getMappedMessageTimeByMessageWidgetPtr(
            dynamic_cast<MessageWidget*>(item));

        return result;
    };

    std::sort(
        list.begin(),
        list.end(),
        [&](auto element1, auto element2)
        {
            return lambda(element1) < lambda(element2);            
        });

    for(auto& element : list)
    {
        auto elementCast = dynamic_cast<MessageWidget*>(element);
        this->chatHistoryLayoutPtr_->removeWidget(elementCast);
        this->chatHistoryLayoutPtr_->addWidget(elementCast);
    }

    // this->chatHistoryLayoutPtr_->update();
}

void CentralPanelWidget::sendMessage(const QString& text)
{
    // auto plainText = this->textEditPtr_->toPlainText();
    // auto text = plainText.toStdString();
    if(!text.isEmpty())
    {
        getGraphicManagerPtr()->sendMessage(this->companionPtr_, text.toStdString());
    }
}

RightPanelWidget::RightPanelWidget(QWidget* parent)
{
    if(parent)
    {
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

RightPanelWidget::~RightPanelWidget()
{
    delete this->layoutPtr_;
    delete this->appLogWidgetPtr_;
    delete this->appLogWidgetPalettePtr_;
}

void RightPanelWidget::set()
{
    this->appLogWidgetPtr_->setParent(this);

    connect(
        this, SIGNAL(addTextToAppLogWidgetSignal(const QString&)),
        this, SLOT(addTextToAppLogWidgetSlot(const QString&)),
        Qt::QueuedConnection);
}

void RightPanelWidget::addTextToAppLogWidgetSlot(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);
    this->appLogWidgetPtr_->ensureCursorVisible();
}

void RightPanelWidget::addTextToAppLogWidget(const QString& text)
{
    emit this->addTextToAppLogWidgetSignal(text);
}

WidgetGroup::WidgetGroup(const Companion* companionPtr) : companionPtr_(companionPtr)
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    const SocketInfo* socketInfoPtr = companionPtr_->getSocketInfoPtr();

    SocketInfoWidget* widget = new SocketInfoWidget(
        const_cast<Companion*>(companionPtr_));

    socketInfoBasePtr_ = dynamic_cast<SocketInfoBaseWidget*>(widget);
    graphicManagerPtr->addWidgetToCompanionPanel(socketInfoBasePtr_);

    centralPanelPtr_ = new CentralPanelWidget(
        getGraphicManagerPtr()->getMainWindowPtr(), companionPtr_->getName());

    centralPanelPtr_->set(const_cast<Companion*>(companionPtr_));

    // buildChatHistory(companionPtr_);
    buildChatHistory();

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, centralPanelPtr_);

    centralPanelPtr_->hide();
}

WidgetGroup::~WidgetGroup()
{
    getGraphicManagerPtr()->removeWidgetFromCompanionPanel(this->socketInfoBasePtr_);
    delete this->socketInfoBasePtr_;
    delete this->centralPanelPtr_;
}

void WidgetGroup::addMessageWidgetToChatHistory(const Message* messagePtr)
{
    this->centralPanelPtr_->addMessageWidgetToChatHistory(
        false, this->companionPtr_->getName(), messagePtr);
}

void WidgetGroup::addMessageWidgetToChatHistoryFromThread(
    bool isAntecedent, const Message* messagePtr)
{
    this->centralPanelPtr_->addMessageWidgetToChatHistoryFromThread(
        isAntecedent, this->companionPtr_->getName(), messagePtr);
}

void WidgetGroup::clearChatHistory()
{
    this->centralPanelPtr_->clearChatHistory();
}

void WidgetGroup::hideCentralPanel()
{
    this->centralPanelPtr_->hide();
}

void WidgetGroup::showCentralPanel()
{
    this->centralPanelPtr_->show();
    this->centralPanelPtr_->scrollDownChatHistory();
}

SocketInfoBaseWidget* WidgetGroup::getSocketInfoBasePtr()
{
    return this->socketInfoBasePtr_;
}

void WidgetGroup::sortChatHistoryElements()
{
    this->centralPanelPtr_->sortChatHistoryElements(true);
}

void WidgetGroup::buildChatHistory()
{
    auto messagePointersPtr = this->companionPtr_->getMessagesPtr();

    for(auto& messagePtr : *messagePointersPtr)
    {
        this->addMessageWidgetToChatHistory(messagePtr);
    }
}

StubWidgetGroup::StubWidgetGroup()
{
    socketInfoPtr_ = new SocketInfoStubWidget;
    leftPanelPtr_ = new LeftPanelWidget(nullptr);
    centralPanelPtr_ = new CentralPanelWidget(nullptr, "");
    rightPanelPtr_ = new RightPanelWidget(nullptr);
}

StubWidgetGroup::~StubWidgetGroup()
{
    delete this->socketInfoPtr_;
    delete this->leftPanelPtr_;
    delete this->centralPanelPtr_;
    delete this->rightPanelPtr_;
}

void StubWidgetGroup::set()
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    SocketInfoBaseWidget* baseObjectCastPtr =
        dynamic_cast<SocketInfoBaseWidget*>(socketInfoPtr_);

    graphicManagerPtr->addWidgetToCompanionPanel(baseObjectCastPtr);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, this->leftPanelPtr_);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::CENTRAL, this->centralPanelPtr_);

    graphicManagerPtr->addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition::RIGHT, this->rightPanelPtr_);

    leftPanelPtr_->hide();
    centralPanelPtr_->hide();
    rightPanelPtr_->hide();
}

void StubWidgetGroup::setParents(
    QWidget* leftContainerPtr, QWidget* centralContainerPtr)
{
    this->leftPanelPtr_->setParent(centralContainerPtr);
    this->centralPanelPtr_->setParent(centralContainerPtr);
    this->rightPanelPtr_->setParent(centralContainerPtr);
}

void StubWidgetGroup::hideSocketInfoStubWidget()
{
    this->socketInfoPtr_->hide();
}

void StubWidgetGroup::hideCentralPanel()
{
    this->centralPanelPtr_->hide();
}

void StubWidgetGroup::showCentralPanel()
{
    this->centralPanelPtr_->show();
}

void StubWidgetGroup::hideStubPanels()
{
    this->leftPanelPtr_->hide();

    if(!getManagerPtr()->isSelectedCompanionNullptr())
    {
        this->centralPanelPtr_->hide();
    }

    this->rightPanelPtr_->hide();
}

void StubWidgetGroup::showStubPanels()
{
    this->leftPanelPtr_->show();
    this->centralPanelPtr_->show();
    this->rightPanelPtr_->show();
}

void StubWidgetGroup::setLeftPanelWidth(int width)
{
    this->leftPanelPtr_->resize(width, this->leftPanelPtr_->height());
}

MainWindowContainerWidget::MainWindowContainerWidget(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        setParent(widgetPtr);
    }

    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setSpacing(0);
    layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);
}

MainWindowContainerWidget::~MainWindowContainerWidget()
{
    delete this->layoutPtr_;
}

void MainWindowContainerWidget::addWidgetToLayout(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        this->layoutPtr_->addWidget(widgetPtr);
    }
}

void MainWindowContainerWidget::addWidgetToLayoutAndSetParentTo(QWidget* widgetPtr)
{
    if(widgetPtr)
    {
        this->layoutPtr_->addWidget(widgetPtr);
        widgetPtr->setParent(this);
    }
}

CompanionDataDialog::CompanionDataDialog(
    CompanionActionType actionType, QWidget* parentPtr, Companion* companionPtr)
{
    setParent(parentPtr);

    setWindowTitle(
        QString::fromStdString(actionTypeStringRepresentation.at(actionType)));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    nameLabelPtr_ = new QLabel("Name");
    nameEditPtr_ = new QLineEdit;

    ipAddressLabelPtr_ = new QLabel("IpAddress");
    ipAddressEditPtr_ = new QLineEdit;

    portLabelPtr_ = new QLabel("Port");
    portEditPtr_ = new QLineEdit;

    if(actionType_ == CompanionActionType::UPDATE && companionPtr)
    {
        nameEditPtr_->setText(QString::fromStdString(companionPtr->getName()));
        ipAddressEditPtr_->setText(QString::fromStdString(companionPtr->getSocketInfoPtr()->getIpAddress()));
        portEditPtr_->setText(QString::fromStdString(std::to_string(companionPtr->getSocketInfoPtr()->getClientPort())));
    }

    layoutPtr_->addRow(nameLabelPtr_, nameEditPtr_);
    layoutPtr_->addRow(ipAddressLabelPtr_, ipAddressEditPtr_);
    layoutPtr_->addRow(portLabelPtr_, portEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CompanionDataDialog::~CompanionDataDialog()
{
    delete this->layoutPtr_;
    delete this->nameLabelPtr_;
    delete this->nameEditPtr_;
    delete this->ipAddressLabelPtr_;
    delete this->ipAddressEditPtr_;
    delete this->portLabelPtr_;
    delete this->portEditPtr_;
    delete this->buttonBoxPtr_;
}

void CompanionDataDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);

    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::rejected,
        this, &QDialog::reject, Qt::QueuedConnection);
}

std::string CompanionDataDialog::getNameString()
{
    return this->nameEditPtr_->text().toStdString();
}

std::string CompanionDataDialog::getIpAddressString()
{
    auto ipAddressFromWidget = this->ipAddressEditPtr_->text().toStdString();  // TODO change
    QHostAddress hostAddress { QString::fromStdString(ipAddressFromWidget) };

    return hostAddress.toString().toStdString();
}

std::string CompanionDataDialog::getPortString()
{
    return this->portEditPtr_->text().toStdString();
}

CreatePasswordDialog::CreatePasswordDialog()
{
    setWindowTitle(newPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    firstLabelPtr_ = new QLabel(newPasswordDialogFirstLabel);
    firstEditPtr_ = new QLineEdit;

    secondLabelPtr_ = new QLabel(newPasswordDialogSecondLabel);
    secondEditPtr_ = new QLineEdit;

    layoutPtr_->addRow(firstLabelPtr_, firstEditPtr_);
    layoutPtr_->addRow(secondLabelPtr_, secondEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CreatePasswordDialog::~CreatePasswordDialog()
{
    delete this->layoutPtr_;
    delete this->firstLabelPtr_;
    delete this->firstEditPtr_;
    delete this->secondLabelPtr_;
    delete this->secondEditPtr_;
    delete this->buttonBoxPtr_;
}

void CreatePasswordDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::getFirstEditText()
{
    return this->firstEditPtr_->text().toStdString();
}

std::string CreatePasswordDialog::getSecondEditText()
{
    return this->secondEditPtr_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog()
{
    setWindowTitle(getPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    labelPtr_ = new QLabel(getPasswordDialogLabel);
    editPtr_ = new QLineEdit;

    layoutPtr_->addRow(labelPtr_, editPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

GetPasswordDialog::~GetPasswordDialog()
{
    delete this->layoutPtr_;
    delete this->labelPtr_;
    delete this->editPtr_;
    delete this->buttonBoxPtr_;
}

void GetPasswordDialog::set()
{
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string GetPasswordDialog::getEditText()
{
    return this->editPtr_->text().toStdString();
}

ButtonInfo::ButtonInfo(
    const QString& buttonText, QDialogButtonBox::ButtonRole buttonRole,
    void (TextDialog::*functionPtr)()) :
    buttonText_(buttonText), buttonRole_(buttonRole), functionPtr_(functionPtr) {}

TextDialog::TextDialog(
    QWidget* parentPtr, DialogType dialogType, const QString& text,
    std::vector<ButtonInfo>* buttonsInfoPtr)
{
    if(parentPtr)
    {
        setParent(parentPtr);
    }
    else
    {
        setParent(getGraphicManagerPtr()->getMainWindowPtr());
    }

    setModal(true);
    setWindowFlag(Qt::Window);

    setWindowTitle(
        QString::fromStdString(dialogTypeStringRepresentation.at(dialogType)));

    layoutPtr_ = new QVBoxLayout;
    setLayout(layoutPtr_);

    textEditPtr_ = new QPlainTextEdit;
    textEditPtr_->setReadOnly(true);
    textEditPtr_->setPlainText(text);
    layoutPtr_->addWidget(textEditPtr_);

    // set button box
    buttonBoxPtr_ = new QDialogButtonBox;
    layoutPtr_->addWidget(buttonBoxPtr_);

    buttonsInfoPtr_ = buttonsInfoPtr;
}

TextDialog::~TextDialog()
{
    delete this->layoutPtr_;
    delete this->textEditPtr_;
    delete this->buttonBoxPtr_;
    delete this->buttonsInfoPtr_;
}

void TextDialog::set()
{
    for(auto& info : *this->buttonsInfoPtr_)
    {
        QPushButton* buttonPtr = this->buttonBoxPtr_->addButton(
            info.buttonText_,
            info.buttonRole_);

        if(info.buttonRole_ == QDialogButtonBox::AcceptRole)
        {
            connect(
                buttonBoxPtr_, &QDialogButtonBox::accepted,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else if(info.buttonRole_ == QDialogButtonBox::RejectRole)
        {
            connect(
                buttonBoxPtr_, &QDialogButtonBox::rejected,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else
        {
            showErrorDialogAndLogError(nullptr, "Unmanaged button role");
        }
    }
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs()
{
    getGraphicManagerPtr()->disableMainWindowBlurEffect();
    this->closeSelfAndParentDialog();
}

void TextDialog::reject()
{
    QDialog::reject();
}

void TextDialog::acceptAction()
{
    this->close();
    this->actionPtr_->sendData();
}

void TextDialog::closeSelf()
{
    this->close();
}

void TextDialog::closeSelfAndParentDialog()
{
    this->close();

    auto parentPtr = this->parent();

    if(parentPtr)
    {
        dynamic_cast<QWidget*>(parentPtr)->close();
    }
}

ShowHideWidget::ShowHideWidget()
{
    show_ = true;
    layoutPtr_ = new QVBoxLayout;
    layoutPtr_->setAlignment(Qt::AlignCenter);
    setLayout(layoutPtr_);

    labelPtr_ = new QLabel;
    labelPtr_->setText("Show / Hide");

    palettePtr_ = new QPalette;
    palettePtr_->setColor(QPalette::Window, QColor(showHideBackGroundColor));
    setAutoFillBackground(true);
    setPalette(*palettePtr_);

    layoutPtr_->addWidget(labelPtr_);
}

ShowHideWidget::~ShowHideWidget()
{
    delete this->layoutPtr_;
    delete this->labelPtr_;
}

void ShowHideWidget::mousePressEvent(QMouseEvent* event)
{
    (this->show_) ? this->hideInfo() : this->showInfo();
    this->show_ = !(this->show_);
}

void ShowHideWidget::hideInfo()
{
    getGraphicManagerPtr()->hideInfo();
}

void ShowHideWidget::showInfo()
{
    getGraphicManagerPtr()->showInfo();
}
