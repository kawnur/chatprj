#include "mainwindow.hpp"

MainWindow* getMainWindowPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->mainWindow_;
}

void MainWindow::setLeftPanel() {
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanel_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    if(leftPanelChildren.size() == 0) {
        this->addStubWidgetToLeftPanel();
    }
    else {
        logArgs(
                    "WARNING: leftPanelChildren.size() != 0 "
                    "at MainWindow::setLeftPanel()");
    }
}

//void MainWindow::setCompanion(QString& name) {
//    this->companion_->setText(name);
//    this->companion_->show();
//}

void MainWindow::addStubWidgetToLeftPanel() {
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    baseObjectCastPtr->setParent(this->leftPanel_);
    this->leftPanelLayout_->addWidget(baseObjectCastPtr);
}

//void MainWindow::addSocketInfoWidgetToLeftPanel(SocketInfo* info) {
void MainWindow::addSocketInfoWidgetToLeftPanel(const Companion* companion) {

    const SocketInfo* socketInfo = companion->getSocketInfo();

    SocketInfoWidget* widget = new SocketInfoWidget(
                companion->getName(),
                socketInfo->getIpaddress(),
                socketInfo->getPort());

    SocketInfoBaseWidget* widgetBase =
            dynamic_cast<SocketInfoBaseWidget*>(widget);

    this->map_[widgetBase] = companion;

    widgetBase->setParent(this->leftPanel_);
    this->leftPanelLayout_->addWidget(widgetBase);
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
        const Companion* companion) const {

    auto findWidget = [&](const std::pair<SocketInfoBaseWidget*, const Companion*>& pair){
        return pair.second == companion;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

const Companion* MainWindow::getMappedCompanionBySocketInfoBaseWidget(
        SocketInfoBaseWidget* widget) const {

    return this->map_.at(widget);
}

void MainWindow::resetSelectedCompanion(const Companion* newSelected) {  // TODO move to manager?
    if(this->selectedCompanion_ != nullptr) {
        auto baseWidget = getMappedSocketInfoBaseWidgetByCompanion(
                    this->selectedCompanion_);

        auto widget = dynamic_cast<SocketInfoWidget*>(baseWidget);
        widget->unselect();

        this->companionNameLabel_->setText("");
        this->companionNameLabel_->hide();

        // TODO get read of buffer, create textwidgets for every companion
        this->selectedCompanion_->setInputBuffer(this->chatHistoryWidget_->toPlainText());

        this->chatHistoryWidget_->setPlainText("");
        this->textEdit_->setText("");
    }

    if(newSelected != nullptr) {
        this->selectedCompanion_ = newSelected;

        auto baseWidget = getMappedSocketInfoBaseWidgetByCompanion(
                    this->selectedCompanion_);

        auto widget = dynamic_cast<SocketInfoWidget*>(baseWidget);
        widget->select();

        this->companionNameLabel_->setText(this->selectedCompanion_->getName());
        this->companionNameLabel_->show();

        auto testString = QString(
                    "Messages from history for " +
                    this->selectedCompanion_->getName());

        this->chatHistoryWidget_->setPlainText(testString);
        this->textEdit_->setText(*this->selectedCompanion_->getInputBuffer());
    }
}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
MainWindow::MainWindow() {
//    setWindowTitle(QString("MainWindow"));
    setWindowTitle(std::getenv("CLIENT_NAME"));
}

void MainWindow::buildSocketInfoWidgets(std::vector<Companion*>* companionsPtr) {
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanel_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    auto companionsSize = companionsPtr->size();
    auto childrenSize = leftPanelChildren.size();

    logArgs("companionsSize:", companionsSize);
    logArgs("childrenSize:", childrenSize);

//    for(auto& i : *companionsPtr) {
//        i.print();
//    }

    for(auto& child : leftPanelChildren) {
        logArgs("child:", child);

        if(child->isStub()) {
            logArgs("child->isStub() == true");
        }
    }

    if(companionsSize == 0) {
        if(childrenSize == 0) {
            logArgs("WARNING: strange case, empty sockets panel");

            this->addStubWidgetToLeftPanel();
        }
    }
    else {
        if(childrenSize != 0) {
            // TODO check if sockets already are children

            for(auto& child : leftPanelChildren) {
                if(child->isStub()) {
                    this->leftPanelLayout_->removeWidget(child);
                    delete child;
                }
            }

            for(auto& i : *companionsPtr) {
//                SocketInfoWidget* widget = new SocketInfoWidget(
//                            i.getName(), i.getIpaddress(), i.getPort());

//                SocketInfoBaseWidget* widgetBase =
//                        dynamic_cast<SocketInfoBaseWidget*>(widget);

//                widgetBase->setParent(this->leftPanel_);
//                this->leftPanelLayout_->addWidget(widgetBase);
                this->addSocketInfoWidgetToLeftPanel(i);
            }
        }
    }
}

void MainWindow::set() {
    this->centralWidget_ = new QWidget(this);
    this->setCentralWidget(centralWidget_);

    this->centralWidgetLayout_ = new QHBoxLayout(this);
    this->centralWidgetLayout_->setSpacing(0);
    this->centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    this->centralWidget_->setLayout(centralWidgetLayout_);

    // left panel

    this->leftPanel_ = new QWidget(this);
    this->leftPanel_->resize(2000, 1000);
    this->leftPanelLayout_ = new QVBoxLayout(this);
    this->leftPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->leftPanelLayout_->setSpacing(0);
    this->leftPanelLayout_->setContentsMargins(0, 0, 0, 0);
    this->leftPanel_->setLayout(leftPanelLayout_);

//    this->test_ = new QLineEdit;
//    this->leftPanelLayout_->addWidget(this->test_);

    this->addStubWidgetToLeftPanel();

    this->centralWidgetLayout_->addWidget(leftPanel_);

    // central panel

    centralPanel_ = new QWidget(this);    
    centralPanelLayout_ = new QVBoxLayout(this);
    centralPanelLayout_->setSpacing(0);
    centralPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centralPanel_->setLayout(centralPanelLayout_);

//    this->companion_ = nullptr;
    this->companionNameLabel_ = new QLabel("", this);
    this->companionNameLabel_->hide();

    centralPanelLayout_->addWidget(this->companionNameLabel_);

    chatHistoryWidget_ = new QPlainTextEdit(this);
    chatHistoryWidget_->setReadOnly(true);
    chatHistoryWidget_->setPlainText("");

    chatHistoryWidgetPalette_ = new QPalette();
    chatHistoryWidgetPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
    chatHistoryWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    chatHistoryWidget_->setAutoFillBackground(true);
    chatHistoryWidget_->setPalette(*chatHistoryWidgetPalette_);

    centralPanelLayout_->addWidget(chatHistoryWidget_);

    textEdit_ = new TextEditWidget(this);
    centralPanelLayout_->addWidget(textEdit_);

    centralWidgetLayout_->addWidget(centralPanel_);

    // right panel

    rightPanel_ = new QWidget(this);
    this->leftPanel_->resize(4000, 1000);
    rightPanelLayout_ = new QVBoxLayout(this);
    rightPanelLayout_->setSpacing(0);
    rightPanelLayout_->setContentsMargins(0, 0, 0, 0);
    rightPanel_->setLayout(rightPanelLayout_);

    appLogWidget_ = new QPlainTextEdit(this);
    appLogWidget_->setReadOnly(true);
    appLogWidget_->setPlainText("");

    appLogWidgetPalette_ = new QPalette();
    appLogWidgetPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
    appLogWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidget_->setAutoFillBackground(true);
    appLogWidget_->setPalette(*appLogWidgetPalette_);

    rightPanelLayout_->addWidget(appLogWidget_);

//    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
//    connect(
//                testPlainTextEditButton_,
//                &QPushButton::clicked,
//                this,
//                &MainWindow::testMainWindowRightPanel);
//    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    centralWidgetLayout_->addWidget(rightPanel_);

//    selectedSocketInfoWidget_ = nullptr;
    selectedCompanion_ = nullptr;

    map_ = std::map<SocketInfoBaseWidget*, const Companion*>();

    // logging enabled, actions
    this->setLeftPanel();
//    this->addTestSocketInfoWidgetToLeftPanel();
}

//int MainWindow::setClient(EchoClient* client) {
//    this->client_ = client;
//    return 0;
//}

//EchoClient*  MainWindow::getClient() {
//    return this->client_;
//}

void MainWindow::closeEvent(QCloseEvent *event) {
    std::exit(0);
}

MainWindow::~MainWindow() {}

void MainWindow::addTextToChatHistoryWidget(const QString& text) {
    this->chatHistoryWidget_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addTextToAppLogWidget(const QString& text) {
    this->appLogWidget_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if(event->key() == Qt::Key_Escape) {
////        if(this->selectedSocketInfoWidget_ != nullptr) {
//        if(this->selectedCompanion_ != nullptr) {
////            this->selectedSocketInfoWidget_->unselect();

//            auto baseWidget = getMappedSocketInfoBaseWidgetByCompanion(
//                        this->selectedCompanion_);

//            auto widget = dynamic_cast<SocketInfoWidget*>(baseWidget);

//            widget->unselect();
//        }

////        this->selectedSocketInfoWidget_ = nullptr;
//        this->selectedCompanion_ = nullptr;

////        QString emptyName { "" };
////        this->setCompanionNameLabel(emptyName);
////        this->companionNameLabel_->hide();

//        this->resetCompanion();
        this->resetSelectedCompanion(nullptr);
    }
}

//void MainWindow::addMessagesToView() {
//    while(this->messages_.size() > 0) {
//    while(this->messages_.size() > 0) {
//        this->addText(this->messages_.front());
//        this->messages_.pop();
//    }
//}

//void addTextFunction(const QString& text) {
//    MainWindow* mainWindow = MainWindow::instance();
//    mainWindow->addText(text);
//}

//void MainWindow::pushMessage(const QString& text) {
//    this->messages_.push(text);
//}

//void MainWindow::pushMessageAndAddText(const QString& text) {
////    this->messages_.push(text);

////    std::thread(addTextFunction, text).detach();
//    this->addText(text);
//}
