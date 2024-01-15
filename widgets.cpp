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

            mainWindow->addTextToCentralPanel(text);
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

SocketInfoWidget::SocketInfoWidget(const QString& name) : name_(name) {}

void SocketInfoWidget::print() {
    logArgs("name:", this->name_);
    logArgs("ipaddress:", this->ipaddress_);
    logArgs("port:", this->port_);
}

bool SocketInfoWidget::isStub() {
    return false;
}

void SocketInfoWidget::set() {
    layout_ = new QHBoxLayout(this);
    indicator_ = new IndicatorWidget(this);
    nameLabel_ = new QLabel(name_, this);
    ipaddressLabel_ = new QLabel(ipaddress_, this);
    portLabel_ = new QLabel(port_, this);
    editButton_ = new QPushButton("Edit", this);

//    toggleIndicatorButton_ = new QPushButton("Toggle Indicator", this);
//    connect(
//                toggleIndicatorButton_, &QPushButton::pressed,
//                indicator_, &IndicatorWidget::toggle);

    layout_->addWidget(indicator_);
    layout_->addWidget(nameLabel_);
    layout_->addWidget(ipaddressLabel_);
    layout_->addWidget(portLabel_);
    layout_->addWidget(editButton_);
//    layout_->addWidget(toggleIndicatorButton_);

    // connect
}

bool SocketInfoStubWidget::isStub() {
    return true;
}
