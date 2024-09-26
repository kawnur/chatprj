#include "widgets.hpp"

void TextEditWidget::keyPressEvent(QKeyEvent* event)
{
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress
            && event->key() == Qt::Key_Return)
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

IndicatorWidget::IndicatorWidget(QWidget* parent)
{
    setParent(parent);

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

// SocketInfoWidget::SocketInfoWidget(QString& name, QString& ipaddress, QString& port) :
//     name_(name), ipaddress_(ipaddress), port_(port) {

//     set();  // TODO move from constructor
// }

// SocketInfoWidget::SocketInfoWidget(QString&& name, QString&& ipaddress, QString&& port) :
//     name_(std::move(name)),
//     ipaddress_(std::move(ipaddress)),
//     port_(std::move(port)) {

//     set();
// }

SocketInfoWidget::SocketInfoWidget(const SocketInfoWidget& si)
{
    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;

    set();
}

SocketInfoWidget::SocketInfoWidget(std::string& name, std::string& ipaddress, std::string& port) :
    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port))
{
    set();
}

SocketInfoWidget::SocketInfoWidget(std::string&& name, std::string&& ipaddress, std::string&& port) :
    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port))
{
    set();
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

void SocketInfoWidget::print()
{
    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

bool SocketInfoWidget::isStub()
{
    return false;
}

void SocketInfoWidget::set()
{
    this->selectedColor_ = QColor(QColorConstants::DarkGray);
    this->unselectedColor_ = QColor(QColorConstants::Gray);
    this->palettePtr_ = new QPalette;  // TODO set parent or delete
    this->palettePtr_->setColor(QPalette::Window, this->unselectedColor_);
    this->setAutoFillBackground(true);
    this->setPalette(*palettePtr_);

    this->layoutPtr_ = new QHBoxLayout(this);
    this->indicatorPtr_ = new IndicatorWidget(this);
    this->nameLabelPtr_ = new QLabel(this->name_, this);
    this->ipaddressLabelPtr_ = new QLabel(this->ipaddress_, this);
    this->portLabelPtr_ = new QLabel(this->port_, this);
    this->editButtonPtr_ = new QPushButton("Edit", this);
    this->connectButtonPtr_ = new QPushButton("Connect", this);

    if(this->name_ == "me")
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
    this->layoutPtr_->addWidget(this->ipaddressLabelPtr_);
    this->layoutPtr_->addWidget(this->portLabelPtr_);
    this->layoutPtr_->addWidget(this->editButtonPtr_);
    this->layoutPtr_->addWidget(this->connectButtonPtr_);
//    layout_->addWidget(toggleIndicatorButton_);

    // connect
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

    set();
}

void SocketInfoStubWidget::set()
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
