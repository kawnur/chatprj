#include "mainwindow.hpp"

WidgetGroup::WidgetGroup(const Companion* companion)
{
    MainWindow* mainWindow = getMainWindowPtr();

    const SocketInfo* socketInfo = companion->getSocketInfo();

    SocketInfoWidget* widget = new SocketInfoWidget(
        companion->getName(),
        socketInfo->getIpaddress(),
        socketInfo->getPort());

    socketInfoBasePtr_ = dynamic_cast<SocketInfoBaseWidget*>(widget);

    mainWindow->addWidgetToLeftPanel(socketInfoBasePtr_);

    chatHistoryPtr_ = new QPlainTextEdit;
    chatHistoryPtr_->setReadOnly(true);
    this->buildChatHistory(companion);
    chatHistoryPtr_->hide();

    chatHistoryPalettePtr_ = new QPalette;  // TODO set parent or delete
    chatHistoryPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    chatHistoryPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    chatHistoryPtr_->setAutoFillBackground(true);
    chatHistoryPtr_->setPalette(*chatHistoryPalettePtr_);

    mainWindow->addWidgetToCentralPanel(chatHistoryPtr_);

    textEditPtr_ = new TextEditWidget;
    textEditPtr_->hide();

    connect(textEditPtr_, &TextEditWidget::send, this, &WidgetGroup::sendMessage);

    mainWindow->addWidgetToCentralPanel(textEditPtr_);
}

void WidgetGroup::sendMessage()
{
    MainWindow* mainWindow = getMainWindowPtr();

    const Companion* companion =
            mainWindow->getMappedCompanionByWidgetGroup(this);

    Manager* manager = getManagerPtr();
    manager->sendMessage(companion, this);
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
    logArgs("message:", "#", msg, "#");

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

MainWindow* getMainWindowPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->mainWindowPtr_;
}

void MainWindow::setLeftPanel()
{
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanelPtr_->findChildren<SocketInfoBaseWidget*>(Qt::FindDirectChildrenOnly);

    if(leftPanelChildren.size() == 0)
    {
        this->addStubWidgetToLeftPanel();
    }
    else
    {
        logArgsWarning(
            "leftPanelChildren.size() != 0 "
            "at MainWindow::setLeftPanel()");
    }
}

void MainWindow::addStubWidgetToLeftPanel()
{
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    baseObjectCastPtr->setParent(this->leftPanelPtr_);
    this->leftPanelLayoutPtr_->addWidget(baseObjectCastPtr);
}

//void MainWindow::testMainWindowRightPanel() {
//    int j = 0;

//    while(true) {
//        coutWithEndl(j);
//        logLine(std::to_string(j));
//        j++;
//        sleep(.1);
////        std::this_thread::sleep_for(std::chrono::milliseconds(200));
//    }
//}

//void MainWindow::addTestSocketInfoWidgetToLeftPanel() {
//    SocketInfo* test1 = new SocketInfo("test1", "test1");
//    SocketInfo* test2 = new SocketInfo("test2", "test2");

//    this->addSocketInfoWidgetToLeftPanel(test1);
//    this->addSocketInfoWidgetToLeftPanel(test2);
//}

//MainWindow* MainWindow::_instance = nullptr;

//MainWindow* MainWindow::instance() {
//    if(_instance == nullptr) {
//        _instance = new MainWindow();
//    }
//    return _instance;
//}

SocketInfoBaseWidget* MainWindow::getMappedSocketInfoBaseWidgetByCompanion(
        const Companion* companion) const
{
    return this->map_.at(companion)->socketInfoBasePtr_;
}

const Companion* MainWindow::getMappedCompanionBySocketInfoBaseWidget(
        SocketInfoBaseWidget* widget) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->socketInfoBasePtr_ == widget;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

const Companion* MainWindow::getMappedCompanionByTextEditWidget(
        TextEditWidget* widget) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->textEditPtr_ == widget;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

const Companion* MainWindow::getMappedCompanionByWidgetGroup(
        WidgetGroup* group) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second == group;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

void MainWindow::resetSelectedCompanion(const Companion* newSelected)
{  // TODO move to manager?
//    logArgs("this->selectedCompanion_:", this->selectedCompanion_);
//    logArgs("newSelected:", newSelected);

    if(this->selectedCompanion_ != nullptr)
    {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->unselect();

        this->companionNameLabelPtr_->setText("");
//        this->companionNameLabel_->hide();

        this->chatHistoryWidgetStubPtr_->setPlainText("");

        widgetGroup->chatHistoryPtr_->hide();
        widgetGroup->textEditPtr_->hide();
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->hide();
        this->textEditStubPtr_->hide();
    }

    this->selectedCompanion_ = newSelected;

    if(newSelected != nullptr)
    {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->select();

        this->companionNameLabelPtr_->setText(
            QString::fromStdString(this->selectedCompanion_->getName()));
        this->companionNameLabelPtr_->show();

        widgetGroup->chatHistoryPtr_->show();
        widgetGroup->textEditPtr_->show();
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->setPlainText("");
        this->chatHistoryWidgetStubPtr_->show();

        this->textEditStubPtr_->setText("");
        this->textEditStubPtr_->show();
    }
}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
MainWindow::MainWindow() {
//    setWindowTitle(QString("MainWindow"));
    setWindowTitle(std::getenv("CLIENT_NAME"));
}

void MainWindow::buildWidgetGroups(std::vector<Companion*>* companionsPtr)
{
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanelPtr_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    auto companionsSize = companionsPtr->size();
    auto childrenSize = leftPanelChildren.size();

    logArgs("companionsSize:", companionsSize);
    logArgs("childrenSize:", childrenSize);

    for(auto& child : leftPanelChildren)
    {
        logArgs("child:", child);

        if(child->isStub())
        {
            logArgs("child->isStub() == true");
        }
    }

    if(companionsSize == 0)
    {
        if(childrenSize == 0)
        {
            logArgsWarning("strange case, empty sockets panel");
            this->addStubWidgetToLeftPanel();
        }
    }
    else
    {
        if(childrenSize != 0)
        {
            // TODO check if sockets already are children

            for(auto& child : leftPanelChildren)
            {
                if(child->isStub())
                {
                    this->leftPanelLayoutPtr_->removeWidget(child);
                    delete child;
                }                
            }

            for(auto& companion : *companionsPtr)
            {
                WidgetGroup* widgetGroup = new WidgetGroup(companion);
                this->map_[companion] = widgetGroup;
            }
        }
    }
}

void MainWindow::set()
{
    // TODO use widget's palette instead of new

    this->centralWidgetPtr_ = new QWidget(this);
    this->setCentralWidget(centralWidgetPtr_);

    this->centralWidgetLayoutPtr_ = new QHBoxLayout(this);
    this->centralWidgetLayoutPtr_->setSpacing(0);
    this->centralWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    this->centralWidgetPtr_->setLayout(centralWidgetLayoutPtr_);

    // left panel

    this->leftPanelPtr_ = new QWidget(this);
    this->leftPanelPtr_->resize(2000, 1000);
    this->leftPanelLayoutPtr_ = new QVBoxLayout(this);
    this->leftPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->leftPanelLayoutPtr_->setSpacing(0);
    this->leftPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    this->leftPanelPtr_->setLayout(leftPanelLayoutPtr_);

//    this->test_ = new QLineEdit;
//    this->leftPanelLayout_->addWidget(this->test_);

    this->addStubWidgetToLeftPanel();

    this->centralWidgetLayoutPtr_->addWidget(leftPanelPtr_);

    // central panel

    this->centralPanelPtr_ = new QWidget(this);
    this->centralPanelLayoutPtr_ = new QVBoxLayout(this);
    this->centralPanelLayoutPtr_->setSpacing(0);
    this->centralPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    this->centralPanelPtr_->setLayout(this->centralPanelLayoutPtr_);

//    this->companion_ = nullptr;
    this->companionNameLabelPtr_ = new QLabel("", this);
    this->companionNameLabelPalettePtr_ = new QPalette();
//    this->companionNameLabelPalette_->setColor(QPalette::Window, QColorConstants::LightGray);
    this->companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(0xa9a9a9));
    this->companionNameLabelPtr_->setAutoFillBackground(true);
    this->companionNameLabelPtr_->setPalette(*this->companionNameLabelPalettePtr_);
//    this->companionNameLabel_->hide();

    this->centralPanelLayoutPtr_->addWidget(this->companionNameLabelPtr_);

    this->chatHistoryWidgetStubPtr_ = new QPlainTextEdit(this);
    this->chatHistoryWidgetStubPtr_->setReadOnly(true);
    this->chatHistoryWidgetStubPtr_->setPlainText("");

    this->chatHistoryWidgetStubPalettePtr_ = new QPalette();
    this->chatHistoryWidgetStubPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
//    chatHistoryWidgetStubPalette_->setColor(QPalette::Text, QColorConstants::Black);
//    chatHistoryWidgetStub_->setAutoFillBackground(true);
    this->chatHistoryWidgetStubPtr_->setPalette(*this->chatHistoryWidgetStubPalettePtr_);

    this->centralPanelLayoutPtr_->addWidget(this->chatHistoryWidgetStubPtr_);

//    this->textEditStub_ = new TextEditWidget(this);
//    centralPanelLayout_->addWidget(this->textEditStub_);

    this->textEditStubPtr_ = new TextEditWidget();
    textEditStubPtr_->setParent(this->centralWidgetPtr_);
    this->centralPanelLayoutPtr_->addWidget(this->textEditStubPtr_);
//    this->textEdit_->hide();

    this->centralWidgetLayoutPtr_->addWidget(this->centralPanelPtr_);

    // right panel

    this->rightPanelPtr_ = new QWidget(this);
    this->leftPanelPtr_->resize(4000, 1000);
    this->rightPanelLayoutPtr_ = new QVBoxLayout(this);
    this->rightPanelLayoutPtr_->setSpacing(0);
    this->rightPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    this->rightPanelPtr_->setLayout(this->rightPanelLayoutPtr_);

    this->appLogWidgetPtr_ = new QPlainTextEdit(this);
    this->appLogWidgetPtr_->setReadOnly(true);
    this->appLogWidgetPtr_->setPlainText("");

    this->appLogWidgetPalettePtr_ = new QPalette();
    this->appLogWidgetPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    this->appLogWidgetPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    this->appLogWidgetPtr_->setAutoFillBackground(true);
    this->appLogWidgetPtr_->setPalette(*this->appLogWidgetPalettePtr_);

    this->rightPanelLayoutPtr_->addWidget(this->appLogWidgetPtr_);

//    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
//    connect(
//                testPlainTextEditButton_,
//                &QPushButton::clicked,
//                this,
//                &MainWindow::testMainWindowRightPanel);
//    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    this->centralWidgetLayoutPtr_->addWidget(this->rightPanelPtr_);

//    selectedSocketInfoWidget_ = nullptr;
    this->selectedCompanion_ = nullptr;

//    map_ = std::map<SocketInfoBaseWidget*, const Companion*>();
//    map_ = std::map<const Companion*, SocketInfoBaseWidget*>();
    this->map_ = std::map<const Companion*, WidgetGroup*>();

    // logging enabled, actions
    this->setLeftPanel();
//    this->addTestSocketInfoWidgetToLeftPanel();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::exit(0);
}

MainWindow::~MainWindow() {}

void MainWindow::addTextToChatHistoryWidget(const QString& text)
{
    this->chatHistoryWidgetStubPtr_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addTextToAppLogWidget(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addWidgetToLeftPanel(SocketInfoBaseWidget* widget)
{
    widget->setParent(this->leftPanelPtr_);
    this->leftPanelLayoutPtr_->addWidget(widget);
}

void MainWindow::addWidgetToCentralPanel(QWidget* widget)
{
    widget->setParent(this->centralPanelPtr_);
    this->centralPanelLayoutPtr_->addWidget(widget);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->resetSelectedCompanion(nullptr);
    }
}
