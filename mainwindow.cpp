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

void MainWindow::addStubWidgetToLeftPanel() {
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    baseObjectCastPtr->setParent(this->leftPanel_);
    this->leftPanelLayout_->addWidget(baseObjectCastPtr);
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

//MainWindow* MainWindow::_instance = nullptr;

//MainWindow* MainWindow::instance() {
//    if(_instance == nullptr) {
//        _instance = new MainWindow();
//    }
//    return _instance;
//}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
MainWindow::MainWindow() {
//    setWindowTitle(QString("MainWindow"));
    setWindowTitle(std::getenv("CLIENT_NAME"));
}

void MainWindow::buildSocketInfoWidgets(std::vector<SocketInfo>* socketsPtr) {
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanel_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    auto socketsSize = socketsPtr->size();
    auto childrenSize = leftPanelChildren.size();

    logArgs("socketsSize:", socketsSize);
    logArgs("childrenSize:", childrenSize);

    for(auto& i : *socketsPtr) {
        i.print();
    }

    for(auto& child : leftPanelChildren) {
        logArgs("child:", child);

        if(child->isStub()) {
            logArgs("child->isStub() == true");
        }
    }

    if(socketsSize == 0) {
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

            for(auto& i : *socketsPtr) {
                SocketInfoWidget* widget = new SocketInfoWidget(
                            i.getName(), i.getIpaddress(), i.getPort());

                SocketInfoBaseWidget* widgetBase =
                        dynamic_cast<SocketInfoBaseWidget*>(widget);

                widgetBase->setParent(this->leftPanel_);
                this->leftPanelLayout_->addWidget(widgetBase);
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

    // logging enabled, actions
    this->setLeftPanel();
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
