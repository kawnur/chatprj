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
//     name_(std::move(name)),
//     ipAddress_(std::move(ipAddress)),
//     port_(std::move(port)) {

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
    indicatorPtr_ = new IndicatorWidget;
    nameLabelPtr_ = new QLabel(name_);
    ipAddressLabelPtr_ = new QLabel(ipAddress_);

    QString serverPortQString = QString::fromStdString(std::to_string(serverPort_));
    QString clientPortQString = QString::fromStdString(std::to_string(clientPort_));

    serverPortLabelPtr_ = new QLabel(serverPortQString);
    clientPortLabelPtr_ = new QLabel(clientPortQString);
    editButtonPtr_ = new QPushButton("Edit");
    connectButtonPtr_ = new QPushButton(getInitialConnectButtonLabel());

    // connect(this->connectButtonPtr_, &QPushButton::clicked, this, &SocketInfoWidget::clientAction);

    if(this->name_ == "me")  // TODO ???
    {
        this->indicatorPtr_->setMe();
        this->editButtonPtr_->hide();
        this->connectButtonPtr_->hide();
    }

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    this->layoutPtr_->addWidget(this->indicatorPtr_);
    this->layoutPtr_->addWidget(this->nameLabelPtr_);
    this->layoutPtr_->addWidget(this->ipAddressLabelPtr_);
    this->layoutPtr_->addWidget(this->serverPortLabelPtr_);
    this->layoutPtr_->addWidget(this->clientPortLabelPtr_);
    this->layoutPtr_->addWidget(this->editButtonPtr_);
    this->layoutPtr_->addWidget(this->connectButtonPtr_);
//    layout_->addWidget(toggleIndicatorButton_);

    // connect
}

void SocketInfoWidget::setParentForChildren()
{
    this->layoutPtr_->setParent(this);
    this->indicatorPtr_->setParent(this);
    this->nameLabelPtr_->setParent(this);
    this->ipAddressLabelPtr_->setParent(this);
    this->serverPortLabelPtr_->setParent(this);
    this->clientPortLabelPtr_->setParent(this);
    this->editButtonPtr_->setParent(this);
    this->connectButtonPtr_->setParent(this);
}

void SocketInfoWidget::setConnections()
{
    connect(
        this->connectButtonPtr_, &QPushButton::clicked,
        this, &SocketInfoWidget::clientAction);
}

void SocketInfoWidget::clientAction()
{
    MainWindow* mainWindow = getMainWindowPtr();

    const Companion* companion =
        mainWindow->getMappedCompanionBySocketInfoBaseWidget(this);

    // if(const_cast<Companion*>(companion)->startClient())
    if(const_cast<Companion*>(companion)->connectClient())
    {
        // change connect button text
        QString currentText = this->connectButtonPtr_->text();
        QString nextText = getNextConnectButtonLabel(currentText);
        this->connectButtonPtr_->setText(nextText);

        // change indicator color
        this->indicatorPtr_->toggle();
    }

    // Manager* manager = getManagerPtr();
    // manager->sendMessage(companion, this);
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
    MainWindow* mainWindow = getMainWindowPtr();

//    auto selectedCompanion = mainWindow->selectedSocketInfoWidget_;

//    if(selected != nullptr) {
//        mainWindow->selectedSocketInfoWidget_->unselect();
//    }

//    mainWindow->selectedSocketInfoWidget_ = this;  // TODO check needed
//    mainWindow
//    this->select();
//    mainWindow->setCompanion(this->name_);

    auto base = dynamic_cast<SocketInfoBaseWidget*>(this);

    auto newCompanion =
            mainWindow->getMappedCompanionBySocketInfoBaseWidget(base);

    mainWindow->resetSelectedCompanion(newCompanion);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent* event) {}

SocketInfoStubWidget::SocketInfoStubWidget()
{
    mark_ = "No socket info from DB...";

    setParentForChildren();
}

void SocketInfoStubWidget::setParentForChildren()
{
    this->layoutPtr_ = new QHBoxLayout(this);
    this->markLabelPtr_ = new QLabel(this->mark_, this);

    this->layoutPtr_->addWidget(this->markLabelPtr_);
}

//SocketInfoStubWidget::~SocketInfoStubWidget() {
//    logArgs("~SocketInfoStubWidget called");
//}

bool SocketInfoStubWidget::isStub()
{
    return true;
}
