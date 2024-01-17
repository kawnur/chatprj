#include "widgets.hpp"

TextEditWidget::TextEditWidget(MainWindow* parent) {
   setParent(parent);
}

void TextEditWidget::keyPressEvent(QKeyEvent* event) {
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress
            && event->key() == Qt::Key_Return) {
        if(event->modifiers() == Qt::NoModifier) {
//            coutWithEndl("enter event");
//            MainWindow* mainWindow = MainWindow::instance();

            MainWindow* mainWindow = getMainWindowPtr();

            auto text = this->toPlainText();

            mainWindow->addTextToChatHistoryWidget(text);
            this->setText("");

//            auto client = mainWindow->getClient();
            ChatClient* client = getChatClientPtr();
            client->send(text);
        }
        else if(event->modifiers() == Qt::ControlModifier) {
            QKeyEvent eventEmulated = QKeyEvent(
                        QEvent::KeyPress,
                        Qt::Key_Return,
                        Qt::NoModifier);

            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

IndicatorWidget::IndicatorWidget(QWidget* parent) {
    setParent(parent);

    setFixedWidth(15);
    setFixedHeight(15);

    isOn_ = false;
    onColor_ = QColor(QColorConstants::DarkGreen);
    offColor_ = QColor(QColorConstants::DarkRed);

    palette_ = new QPalette();
    palette_->setColor(QPalette::Window, offColor_);

    setAutoFillBackground(true);
    setPalette(*palette_);
}

IndicatorWidget::IndicatorWidget(const IndicatorWidget* indicator) {
    isOn_ = indicator->isOn_;
    onColor_ = indicator->onColor_;
    offColor_ = indicator->offColor_;
    palette_ = indicator->palette_;
}

void IndicatorWidget::setOn() {
    this->isOn_ = true;
    this->palette_->setColor(QPalette::Window, onColor_);
    this->setPalette(*this->palette_);
}

void IndicatorWidget::setOff() {
    this->isOn_ = false;
    this->palette_->setColor(QPalette::Window, offColor_);
    this->setPalette(*this->palette_);
}

void IndicatorWidget::toggle() {
    (this->isOn_ == true) ? this->setOff() : this->setOn();
}

SocketInfoWidget::SocketInfoWidget() {
    logArgs("SocketInfoWidget()");
}

SocketInfoWidget::SocketInfoWidget(QString& name, QString& ipaddress, QString& port) :
    name_(name), ipaddress_(ipaddress), port_(port) {

    set();
}

SocketInfoWidget::SocketInfoWidget(QString&& name, QString&& ipaddress, QString&& port) :
    name_(std::move(name)),
    ipaddress_(std::move(ipaddress)),
    port_(std::move(port)) {

    set();
}

SocketInfoWidget::SocketInfoWidget(const SocketInfoWidget& si) {
    name_ = si.name_;
    ipaddress_ = si.ipaddress_;
    port_ = si.port_;

    set();
}

SocketInfoWidget::SocketInfoWidget(std::string& name, std::string& ipaddress, std::string& port) :
    name_(QString::fromStdString(name)),
    ipaddress_(QString::fromStdString(ipaddress)),
    port_(QString::fromStdString(port)) {

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

void SocketInfoWidget::print() {
    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

bool SocketInfoWidget::isStub() {
    return false;
}

void SocketInfoWidget::set() {
    this->selectedColor_ = QColor(QColorConstants::DarkGray);
    this->unselectedColor_ = QColor(QColorConstants::Gray);
    this->palette_ = new QPalette();
    this->palette_->setColor(QPalette::Window, this->unselectedColor_);
    this->setAutoFillBackground(true);
    this->setPalette(*palette_);

    this->layout_ = new QHBoxLayout(this);
    this->indicator_ = new IndicatorWidget(this);
    this->nameLabel_ = new QLabel(this->name_, this);
    this->ipaddressLabel_ = new QLabel(this->ipaddress_, this);
    this->portLabel_ = new QLabel(this->port_, this);
    this->editButton_ = new QPushButton("Edit", this);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    this->layout_->addWidget(this->indicator_);
    this->layout_->addWidget(this->nameLabel_);
    this->layout_->addWidget(this->ipaddressLabel_);
    this->layout_->addWidget(this->portLabel_);
    this->layout_->addWidget(this->editButton_);
//    layout_->addWidget(toggleIndicatorButton_);

    // connect
}

void SocketInfoWidget::changeColor(QColor& color) {
    this->palette_ = new QPalette();  // TODO leak
    this->palette_->setColor(QPalette::Window, color);

    this->setAutoFillBackground(true);
    this->setPalette(*this->palette_);
}

void SocketInfoWidget::select() {
    this->changeColor(this->selectedColor_);
}

void SocketInfoWidget::unselect() {
    this->changeColor(this->unselectedColor_);
}

void SocketInfoWidget::mousePressEvent(QMouseEvent* event) {
    MainWindow* mainWindow = getMainWindowPtr();

    auto selected = mainWindow->selectedSocketInfoWidget_;

    if(selected != nullptr) {
        mainWindow->selectedSocketInfoWidget_->unselect();
    }

    mainWindow->selectedSocketInfoWidget_ = this;
    this->select();
    mainWindow->setCompanion(this->name_);
}

void SocketInfoWidget::mouseReleaseEvent(QMouseEvent* event) {}

SocketInfoStubWidget::SocketInfoStubWidget() {
    mark_ = "No socket info from DB...";

    set();
}

void SocketInfoStubWidget::set() {
    this->layout_ = new QHBoxLayout(this);
    this->markLabel_ = new QLabel(this->mark_, this);

    this->layout_->addWidget(this->markLabel_);
}

//SocketInfoStubWidget::~SocketInfoStubWidget() {
//    logArgs("~SocketInfoStubWidget called");
//}

bool SocketInfoStubWidget::isStub() {
    return true;
}
