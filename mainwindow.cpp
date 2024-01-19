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

void MainWindow::addSocketInfoWidgetToLeftPanel(const Companion* companion) {
    const SocketInfo* socketInfo = companion->getSocketInfo();

    SocketInfoWidget* widget = new SocketInfoWidget(
                companion->getName(),
                socketInfo->getIpaddress(),
                socketInfo->getPort());

    SocketInfoBaseWidget* widgetBase =
            dynamic_cast<SocketInfoBaseWidget*>(widget);

    this->map_[companion]->socketInfoBase_ = widgetBase;

    widgetBase->setParent(this->leftPanel_);
    this->leftPanelLayout_->addWidget(widgetBase);
}

void MainWindow::addChatHistoryWidgetToMapping(const Companion* companion) {
    QPlainTextEdit* chatHistoryWidget_ = new QPlainTextEdit(this->centralPanel_);
    chatHistoryWidget_->setReadOnly(true);
    chatHistoryWidget_->setPlainText("");
    chatHistoryWidget_->hide();

    this->map_[companion]->chatHistory_->textEdit_ = chatHistoryWidget_;

    QPalette* chatHistoryWidgetPalette_ = new QPalette();
    chatHistoryWidgetPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
    chatHistoryWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    chatHistoryWidget_->setAutoFillBackground(true);
    chatHistoryWidget_->setPalette(*chatHistoryWidgetPalette_);

    this->map_[companion]->chatHistory_->palette_ = chatHistoryWidgetPalette_;

    this->centralPanelLayout_->addWidget(chatHistoryWidget_);
}

void MainWindow::addTextEditWidgetToMapping(const Companion* companion) {
    TextEditWidget* textEdit = new TextEditWidget(this);
    textEdit->hide();
    textEdit->setParent(this->centralPanel_);

    this->map_[companion]->textEdit_ = textEdit;

    this->centralPanelLayout_->addWidget(textEdit);
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

    return this->map_.at(companion)->socketInfoBase_;
}

const Companion* MainWindow::getMappedCompanionBySocketInfoBaseWidget(
        SocketInfoBaseWidget* widget) const {

    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->socketInfoBase_ == widget;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

void MainWindow::resetSelectedCompanion(const Companion* newSelected) {  // TODO move to manager?
//    logArgs("this->selectedCompanion_:", this->selectedCompanion_);
//    logArgs("newSelected:", newSelected);

    if(this->selectedCompanion_ != nullptr) {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBase_)->unselect();

        this->companionNameLabel_->setText("");
//        this->companionNameLabel_->hide();

        this->chatHistoryWidgetStub_->setPlainText("");

        widgetGroup->chatHistory_->textEdit_->hide();
        widgetGroup->textEdit_->hide();
    }
    else {
        this->chatHistoryWidgetStub_->hide();
        this->textEditStub_->hide();
    }

    this->selectedCompanion_ = newSelected;

    if(newSelected != nullptr) {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBase_)->select();

        this->companionNameLabel_->setText(this->selectedCompanion_->getName());
        this->companionNameLabel_->show();

        auto testString = QString(
                    "Messages from history for " +
                    this->selectedCompanion_->getName());

        widgetGroup->chatHistory_->textEdit_->setPlainText(testString);

        widgetGroup->chatHistory_->textEdit_->show();
        widgetGroup->textEdit_->show();
    }
    else {
        this->chatHistoryWidgetStub_->setPlainText("");
        this->chatHistoryWidgetStub_->show();

        this->textEditStub_->setText("");
        this->textEditStub_->show();
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
                WidgetGroup* widgetGroup = new WidgetGroup;
                this->map_[i] = widgetGroup;

                ChatHistoryGroup* chatHistoryGroup = new ChatHistoryGroup;
                this->map_[i]->chatHistory_ = chatHistoryGroup;

                this->addSocketInfoWidgetToLeftPanel(i);
                this->addChatHistoryWidgetToMapping(i);
                this->addTextEditWidgetToMapping(i);
            }
        }
    }
}

void MainWindow::set() {

    // TODO use widget's palette instead of new

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

    this->centralPanel_ = new QWidget(this);
    this->centralPanelLayout_ = new QVBoxLayout(this);
    this->centralPanelLayout_->setSpacing(0);
    this->centralPanelLayout_->setContentsMargins(0, 0, 0, 0);
    this->centralPanel_->setLayout(this->centralPanelLayout_);

//    this->companion_ = nullptr;
    this->companionNameLabel_ = new QLabel("", this);
    this->companionNameLabelPalette_ = new QPalette();
//    this->companionNameLabelPalette_->setColor(QPalette::Window, QColorConstants::LightGray);
    this->companionNameLabelPalette_->setColor(QPalette::Window, QColor(0xa9a9a9));
    this->companionNameLabel_->setAutoFillBackground(true);
    this->companionNameLabel_->setPalette(*this->companionNameLabelPalette_);
//    this->companionNameLabel_->hide();

    this->centralPanelLayout_->addWidget(this->companionNameLabel_);

    this->chatHistoryWidgetStub_ = new QPlainTextEdit(this);
    this->chatHistoryWidgetStub_->setReadOnly(true);
    this->chatHistoryWidgetStub_->setPlainText("");

    this->chatHistoryWidgetStubPalette_ = new QPalette();
    this->chatHistoryWidgetStubPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
//    chatHistoryWidgetStubPalette_->setColor(QPalette::Text, QColorConstants::Black);
//    chatHistoryWidgetStub_->setAutoFillBackground(true);
    this->chatHistoryWidgetStub_->setPalette(*this->chatHistoryWidgetStubPalette_);

    this->centralPanelLayout_->addWidget(this->chatHistoryWidgetStub_);

//    this->textEditStub_ = new TextEditWidget(this);
//    centralPanelLayout_->addWidget(this->textEditStub_);

    this->textEditStub_ = new TextEditWidget(this);
    this->centralPanelLayout_->addWidget(this->textEditStub_);
//    this->textEdit_->hide();

    this->centralWidgetLayout_->addWidget(this->centralPanel_);

    // right panel

    this->rightPanel_ = new QWidget(this);
    this->leftPanel_->resize(4000, 1000);
    this->rightPanelLayout_ = new QVBoxLayout(this);
    this->rightPanelLayout_->setSpacing(0);
    this->rightPanelLayout_->setContentsMargins(0, 0, 0, 0);
    this->rightPanel_->setLayout(this->rightPanelLayout_);

    this->appLogWidget_ = new QPlainTextEdit(this);
    this->appLogWidget_->setReadOnly(true);
    this->appLogWidget_->setPlainText("");

    this->appLogWidgetPalette_ = new QPalette();
    this->appLogWidgetPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
    this->appLogWidgetPalette_->setColor(QPalette::Text, QColorConstants::Black);
    this->appLogWidget_->setAutoFillBackground(true);
    this->appLogWidget_->setPalette(*this->appLogWidgetPalette_);

    this->rightPanelLayout_->addWidget(this->appLogWidget_);

//    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
//    connect(
//                testPlainTextEditButton_,
//                &QPushButton::clicked,
//                this,
//                &MainWindow::testMainWindowRightPanel);
//    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    this->centralWidgetLayout_->addWidget(this->rightPanel_);

//    selectedSocketInfoWidget_ = nullptr;
    this->selectedCompanion_ = nullptr;

//    map_ = std::map<SocketInfoBaseWidget*, const Companion*>();
//    map_ = std::map<const Companion*, SocketInfoBaseWidget*>();
    this->map_ = std::map<const Companion*, WidgetGroup*>();

    // logging enabled, actions
    this->setLeftPanel();
//    this->addTestSocketInfoWidgetToLeftPanel();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    std::exit(0);
}

MainWindow::~MainWindow() {}

void MainWindow::addTextToChatHistoryWidget(const QString& text) {
    this->chatHistoryWidgetStub_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addTextToAppLogWidget(const QString& text) {
    this->appLogWidget_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if(event->key() == Qt::Key_Escape) {
        this->resetSelectedCompanion(nullptr);
    }
}
