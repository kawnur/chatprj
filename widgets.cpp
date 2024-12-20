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
            this->send();
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

IndicatorWidget::IndicatorWidget()
{
    setFixedWidth(15);
    setFixedHeight(15);

    isOn_ = false;
    onColor_ = QColor(QColorConstants::DarkGreen);
    offColor_ = QColor(QColorConstants::DarkRed);
    meColor_ = QColor(indicatorMeColor);

    palettePtr_ = new QPalette;  // TODO set parent or delete
    palettePtr_->setColor(QPalette::Window, offColor_);

    setAutoFillBackground(true);
    setPalette(*palettePtr_);
}

IndicatorWidget::IndicatorWidget(const IndicatorWidget* indicator)
{
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palettePtr_ = indicator->palettePtr_;
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
    indicatorPtr_ = new IndicatorWidget;
    nameLabelPtr_ = new QLabel(name_);
    ipAddressLabelPtr_ = new QLabel(ipAddress_);

    QString serverPortQString = QString::fromStdString(std::to_string(serverPort_));
    QString clientPortQString = QString::fromStdString(std::to_string(clientPort_));

    serverPortLabelPtr_ = new QLabel(serverPortQString);
    clientPortLabelPtr_ = new QLabel(clientPortQString);
    editButtonPtr_ = new QPushButton("Edit");
    connectButtonPtr_ = new QPushButton(getInitialConnectButtonLabel());

    connect(this->editButtonPtr_, &QPushButton::clicked, this, &SocketInfoWidget::updateCompanionAction);
    connect(this->connectButtonPtr_, &QPushButton::clicked, this, &SocketInfoWidget::clientAction);

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
    connect(this, &QWidget::customContextMenuRequested, this, &SocketInfoWidget::slotCustomMenuRequested);

    // connect
}

void SocketInfoWidget::slotCustomMenuRequested(QPoint pos)
{
    QMenu* menu = new QMenu(this);

    QAction* clearHistoryAction = new QAction("Clear chat history", this);
    menu->addAction(clearHistoryAction);
    connect(clearHistoryAction, &QAction::triggered, this, &SocketInfoWidget::clearHistoryAction);

    QAction* deleteCompanionAction = new QAction("Delete companion", this);
    menu->addAction(deleteCompanionAction);
    connect(deleteCompanionAction, &QAction::triggered, this, &SocketInfoWidget::deleteCompanionAction);

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

    if(currentText == CONNECT_BUTTON_CONNECT_LABEL)
    {
        result = const_cast<Companion*>(companion)->connectClient();
    }
    else if(currentText == CONNECT_BUTTON_DISCONNECT_LABEL)
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
    this->palettePtr_ = new QPalette;  // TODO set parent or delete
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
    mark_ = QString::fromStdString(socketInfoStubWidget);

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);

    markLabelPtr_ = new QLabel(mark_);
    layoutPtr_->addWidget(markLabelPtr_);
}

bool SocketInfoStubWidget::isStub()
{
    return true;
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

void LeftPanelWidget::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)  // TODO do we nedd remove?
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    qsizetype index = companionPanelChildren.indexOf(widgetPtr);

    if(index == -1)
    {
        showErrorDialogAndLogError("SocketInfoBaseWidget was not found in companion panel");
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

CentralPanelWidget::CentralPanelWidget(QWidget* parent, const std::string& name)
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

    chatHistoryWidgetPtr_ = new QPlainTextEdit;
    chatHistoryWidgetPtr_->setReadOnly(true);
    chatHistoryWidgetPtr_->setPlainText("");

    chatHistoryWidgetPalettePtr_ = new QPalette;
    chatHistoryWidgetPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    chatHistoryWidgetPtr_->setPalette(*chatHistoryWidgetPalettePtr_);

    layoutPtr_->addWidget(chatHistoryWidgetPtr_);

    textEditPtr_ = new TextEditWidget;    
    layoutPtr_->addWidget(textEditPtr_);
}

CentralPanelWidget::~CentralPanelWidget()
{
    delete this->layoutPtr_;
    delete this->companionNameLabelPtr_;
    delete this->companionNameLabelPalettePtr_;
    delete this->chatHistoryWidgetPtr_;
    delete this->chatHistoryWidgetPalettePtr_;
    delete this->textEditPtr_;
    delete this->textEditPalettePtr_;
}

void CentralPanelWidget::set(Companion* companionPtr)
{
    this->companionPtr_ = companionPtr;
    connect(this->textEditPtr_, &TextEditWidget::send, this, &CentralPanelWidget::sendMessage);
}

void CentralPanelWidget::addMessageToChatHistory(const QString& message)
{
    this->chatHistoryWidgetPtr_->appendPlainText(message);
}

void CentralPanelWidget::clearChatHistory()
{
    this->chatHistoryWidgetPtr_->clear();
}

void CentralPanelWidget::sendMessage()
{
    auto text = this->textEditPtr_->toPlainText().toStdString();
    getGraphicManagerPtr()->sendMessage(this->companionPtr_, text);
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
}

void RightPanelWidget::addTextToAppLogWidget(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);
}

WidgetGroup::WidgetGroup(const Companion* companionPtr)
{
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    const SocketInfo* socketInfoPtr = companionPtr->getSocketInfoPtr();

    SocketInfoWidget* widget = new SocketInfoWidget(
        const_cast<Companion*>(companionPtr));

    socketInfoBasePtr_ = dynamic_cast<SocketInfoBaseWidget*>(widget);
    graphicManagerPtr->addWidgetToCompanionPanel(socketInfoBasePtr_);

    centralPanelPtr_ = new CentralPanelWidget(
        getGraphicManagerPtr()->getMainWindowPtr(), companionPtr->getName());

    centralPanelPtr_->set(const_cast<Companion*>(companionPtr));

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

QString WidgetGroup::formatMessage(const Companion* companion, const Message* message)  // TODO move to utils
{
    auto companionId = message->getCompanionId();
    auto authorId = message->getAuthorId();
    auto time = QString::fromStdString(message->getTime());
    auto text = QString::fromStdString(message->getText());
    auto isSent = message->getIsSent();

    QString sender = (companionId == authorId)
                         ? QString::fromStdString(companion->getName()) : "Me";

    QString receiver = (companionId == authorId)
                           ? "Me" : QString::fromStdString(companion->getName());

    QString prefix = QString("From %1 to %2 at %3:\n")
                         .arg(sender, receiver, time);

    QString messageIndent = "      ";

    text.replace("\n", "\n" + messageIndent);

    QString msg = prefix + messageIndent + text + QString("\n");

    return msg;
}

void WidgetGroup::addMessageToChatHistory(const QString& message)
{
    this->centralPanelPtr_->addMessageToChatHistory(message);
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
}

SocketInfoBaseWidget* WidgetGroup::getSocketInfoBasePtr()
{
    return this->socketInfoBasePtr_;
}

QString WidgetGroup::buildChatHistory(const Companion* companion)
{
    const std::vector<Message>* messages = companion->getMessagesPtr();
    QString result { "" };

    for(auto& message : *messages)
    {
        this->addMessageToChatHistory(
            this->formatMessage(companion, &message));
    }

    return result;
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

void CompanionDataDialog::set(CompanionAction* actionPtr)
{
    this->actionPtr_ = actionPtr;

    connect(this->buttonBoxPtr_, &QDialogButtonBox::accepted, this->actionPtr_, &CompanionAction::sendData);
    connect(this->buttonBoxPtr_, &QDialogButtonBox::rejected, this, &QDialog::reject);
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

TextDialog::TextDialog(
    QDialog* parentDialog, QWidget* parent,
    DialogType dialogType, const std::string& text)
{
    setParent(parent);

    setModal(true);
    setWindowFlag(Qt::Window);

    this->setWindowTitle(
        QString::fromStdString(dialogTypeStringRepresentation.at(dialogType)));

    parentDialogPtr_ = parentDialog;

    layoutPtr_ = new QVBoxLayout;
    setLayout(layoutPtr_);

    textEditPtr_ = new QPlainTextEdit;
    textEditPtr_->setReadOnly(true);

    textEditPtr_->setPlainText(QString::fromStdString(text));

    layoutPtr_->addWidget(textEditPtr_);
}

TextDialog::~TextDialog()
{
    delete this->layoutPtr_;
    delete this->textEditPtr_;
    delete this->buttonBoxPtr_;
}

void TextDialog::set(QDialog* parentDialog)
{
    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);

    if(parentDialog)
    {
        connect(buttonBoxPtr_, &QDialogButtonBox::accepted, this, &TextDialog::closeDialogs);
    }
    else
    {
        connect(buttonBoxPtr_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    }

    layoutPtr_->addWidget(buttonBoxPtr_);
}

void TextDialog::closeDialogs()
{
    this->close();
    this->parentDialogPtr_->close();
}

TwoButtonsTextDialog::TwoButtonsTextDialog(
    QDialog* parentDialog, QWidget* parent, DialogType dialogType,
    const std::string& text, std::string&& buttonText) :
    TextDialog(parentDialog, parent, dialogType, text), buttonText_(std::move(buttonText)) {}

void TwoButtonsTextDialog::set(CompanionAction* actionPtr)
{
    this->actionPtr_ = actionPtr;

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Cancel);

    buttonBoxPtr_->addButton(
        QString::fromStdString(this->buttonText_), QDialogButtonBox::AcceptRole);

    connect(this->buttonBoxPtr_, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(this->buttonBoxPtr_, &QDialogButtonBox::accepted, this, &TwoButtonsTextDialog::acceptAction);

    layoutPtr_->addWidget(buttonBoxPtr_);
}

void TwoButtonsTextDialog::acceptAction()  // TODO rename to accept and overload
{
    this->close();
    this->actionPtr_->sendData();
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
