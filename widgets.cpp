#include "widgets.hpp"

QString getInitialConnectButtonLabel()
{
    try
    {
        if(connectButtonLabels.empty())
        {
            return QString("");
        }
        else
        {
            return connectButtonLabels.at(0);
        }
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
    meColor_ = QColor(0x6a6a6a);

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

// SocketInfoWidget::SocketInfoWidget(QString& name, QString& ipAddress, QString& port) :
//     name_(name), ipAddress_(ipAddress), port_(port) {

//     set();  // TODO move from constructor
// }

// SocketInfoWidget::SocketInfoWidget(QString&& name, QString&& ipAddress, QString&& port) :
//     name_(name),
//     ipAddress_(ipAddress),
//     port_(port) {

//     setParentForChildren();
// }

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

//SocketInfoWidget::SocketInfoWidget(const QString& name) : name_(name) {
////    logArgs("SocketInfoWidget(const QString& name)");

//    set();

//    IndicatorWidget* oldIndicator = indicator_;
//    QPushButton* oldEditButton = editButton_;

//    indicator_ = nullptr;
//    editButton_ = nullptr;

//    layout_->removeWidget(oldIndicator);
//    layout_->removeWidget(oldEditButton);

//    delete oldIndicator;
//    delete oldEditButton;
//}

SocketInfoWidget::~SocketInfoWidget()
{
    // cannot set parent
    delete this->palettePtr_;
}


void SocketInfoWidget::print()
{
    logArgs(
        "name:", this->name_,
        "ipAddress:", this->ipAddress_,
        "serverPort_:", this->serverPort_,
        "clientPort_:", this->clientPort_
    );
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

    // connect
}

void SocketInfoWidget::clientAction()
{
    bool result = false;
    // Manager* managerPtr = getManagerPtr();

    // const Companion* companion =
    //     managerPtr->getMappedCompanionBySocketInfoBaseWidget(this);

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
        // QString currentText = this->connectButtonPtr_->text();
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
    mark_ = "No socket info from DB...";

    layoutPtr_ = new QHBoxLayout;
    setLayout(layoutPtr_);

    markLabelPtr_ = new QLabel(mark_);
    layoutPtr_->addWidget(markLabelPtr_);
}

bool SocketInfoStubWidget::isStub()
{
    return true;
}

WidgetGroup::WidgetGroup(const Companion* companion)
{
    // MainWindow* mainWindowPtr = getMainWindowPtr();
    GraphicManager* graphicManagerPtr = getGraphicManagerPtr();

    const SocketInfo* socketInfoPtr = companion->getSocketInfoPtr();

    SocketInfoWidget* widget = new SocketInfoWidget(
        companion->getName(),
        socketInfoPtr->getIpAddress(),
        socketInfoPtr->getServerPort(),
        socketInfoPtr->getClientPort());

    socketInfoBasePtr_ = dynamic_cast<SocketInfoBaseWidget*>(widget);
    // mainWindowPtr->addWidgetToLeftPanel(socketInfoBasePtr_);
    graphicManagerPtr->addWidgetToLeftPanel(socketInfoBasePtr_);

    chatHistoryPtr_ = new QPlainTextEdit;
    // chatHistoryPtr_->setStyleSheet("border-bottom: 1px solid black");
    chatHistoryPtr_->setReadOnly(true);
    buildChatHistory(companion);
    chatHistoryPtr_->hide();

    chatHistoryPalettePtr_ = new QPalette;  // TODO set parent or delete
    chatHistoryPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    chatHistoryPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    chatHistoryPtr_->setAutoFillBackground(true);
    chatHistoryPtr_->setPalette(*chatHistoryPalettePtr_);

    // mainWindowPtr->addWidgetToCentralPanel(chatHistoryPtr_);
    graphicManagerPtr->addWidgetToCentralPanel(chatHistoryPtr_);

    textEditPtr_ = new TextEditWidget;
    textEditPtr_->hide();

    connect(textEditPtr_, &TextEditWidget::send, this, &WidgetGroup::sendMessage);

    // mainWindowPtr->addWidgetToCentralPanel(textEditPtr_);
    graphicManagerPtr->addWidgetToCentralPanel(textEditPtr_);
}

void WidgetGroup::sendMessage()
{
    // Manager* managerPtr = getManagerPtr();

    auto text = this->textEditPtr_->toPlainText().toStdString();

    // const Companion* companion =
    //     manager->getMappedCompanionByWidgetGroup(this);

    // manager->sendMessage(companion, this);
    // managerPtr->sendMessage(this, text);
    getManagerPtr()->sendMessage(this, text);
}

QString WidgetGroup::formatMessage(const Companion* companion, const Message* message)
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
    // logArgs("message:", "#", msg, "#");

    return msg;
}

void WidgetGroup::addMessageToChatHistory(const QString& message)
{
    this->chatHistoryPtr_->appendPlainText(message);
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

// InputFormElementWidget::InputFormElementWidget(std::string&& label)
// {
//     label_ = label;

//     layoutPtr_ = new QHBoxLayout;

//     labelPtr_ = new QLabel(QString::fromStdString(label_));
//     editPtr_ = new QLineEdit;

//     layoutPtr_->addWidget(labelPtr_);
//     layoutPtr_->addWidget(editPtr_);
// }

// InputFormElementWidget::~InputFormElementWidget()
// {
//     // didn't set parent for members
//     delete this->layoutPtr_;
//     delete this->labelPtr_;
//     delete this->editPtr_;
// }

NewCompanionDialog::NewCompanionDialog(QWidget* parent)
{
    // setParent(parent);

    setWindowTitle("Add new companion");

    // setModal(true);
    // setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    // layoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // layoutPtr_->setSpacing(0);
    // layoutPtr_->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutPtr_);

    nameLabelPtr_ = new QLabel("Name");
    nameEditPtr_ = new QLineEdit;

    ipAddressLabelPtr_ = new QLabel("IpAddress");
    ipAddressEditPtr_ = new QLineEdit;

    portLabelPtr_ = new QLabel("Port");
    portEditPtr_ = new QLineEdit;

    layoutPtr_->addRow(nameLabelPtr_, nameEditPtr_);
    layoutPtr_->addRow(ipAddressLabelPtr_, ipAddressEditPtr_);
    layoutPtr_->addRow(portLabelPtr_, portEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBoxPtr_, &QDialogButtonBox::accepted, this, &NewCompanionDialog::sendData);  // TODO this in ctor
    connect(buttonBoxPtr_, &QDialogButtonBox::rejected, this, &QDialog::reject);  // TODO this in ctor

    layoutPtr_->addWidget(buttonBoxPtr_);
}

NewCompanionDialog::~NewCompanionDialog()
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

void NewCompanionDialog::sendData()
{
    auto name = this->nameEditPtr_->text().toStdString();
    auto ipAddress = this->ipAddressEditPtr_->text().toStdString();
    auto port = this->portEditPtr_->text().toStdString();

    logArgs("name", name);
    logArgs("ipAddress", ipAddress);
    logArgs("port", port);

    getManagerPtr()->addNewCompanion(name, ipAddress, port);
}

WarningDialog::WarningDialog(QWidget* parent)
{
    setParent(parent);

    setModal(true);
    setWindowFlag(Qt::Window);

    this->setWindowTitle("WARNING");

    layoutPtr_ = new QVBoxLayout;
    setLayout(layoutPtr_);

    textEditPtr_ = new QPlainTextEdit;
    textEditPtr_->setReadOnly(true);
    textEditPtr_->setPlainText("");
    layoutPtr_->addWidget(textEditPtr_);

    QDialogButtonBox* buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBoxPtr_, &QDialogButtonBox::accepted, this, &QDialog::accept);  // TODO this in ctor
    layoutPtr_->addWidget(buttonBoxPtr_);
}

WarningDialog::~WarningDialog()
{
    delete this->layoutPtr_;
    delete this->textEditPtr_;
    delete this->buttonBoxPtr_;
}
