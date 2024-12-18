#include "mainwindow.hpp"

// MainWindow* getMainWindowPtr()
// {
//     QCoreApplication* coreApp = QCoreApplication::instance();
//     ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
//     return app->mainWindowPtr_;
// }

void MainWindow::addStubWidgetToCompanionPanel()
{
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    this->companionPanelLayoutPtr_->addWidget(baseObjectCastPtr);
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

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
MainWindow::MainWindow()
{
    setWindowTitle(std::getenv("CLIENT_NAME"));

    // menu bar

    menuBarPalettePtr_ = new QPalette;
    menuBarPalettePtr_->setColor(
        QPalette::Window, QColor(mainWindowMenuBarBackgroundColor));
    menuBar()->setAutoFillBackground(true);
    menuBar()->setPalette(*menuBarPalettePtr_);

    // central widget

    centralWidgetPtr_ = new QWidget;
    setCentralWidget(centralWidgetPtr_);

    centralWidgetLayoutPtr_ = new QHBoxLayout;
    centralWidgetLayoutPtr_->setSpacing(0);
    centralWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    centralWidgetPtr_->setLayout(centralWidgetLayoutPtr_);

    // left panel

    leftPanelPtr_ = new LeftPanelWidget(centralWidgetPtr_);

    // leftPanelWidgetPtr_ = new LeftPanelWidget;

    // leftPanelPtr_ = new QWidget(centralWidgetPtr_);
    // leftPanelLayoutPtr_ = new QVBoxLayout(leftPanelPtr_);
    // leftPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // leftPanelLayoutPtr_->setSpacing(0);
    // leftPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);

    // leftPanelPalettePtr_ = new QPalette;
    // leftPanelPalettePtr_->setColor(QPalette::Window, QColor(leftPanelBackgroundColor));
    // leftPanelPtr_->setAutoFillBackground(true);
    // leftPanelPtr_->setPalette(*leftPanelPalettePtr_);

    // leftPanelPtr_->setLayout(leftPanelLayoutPtr_);

    centralWidgetLayoutPtr_->addWidget(leftPanelPtr_);

    // companionPanelPtr_ = new QWidget(leftPanelPtr_);
    // // leftPanelPtr_->setStyleSheet("border-right: 1px solid black");
    // companionPanelPtr_->resize(2000, 1000);  // TODO ???
    // companionPanelLayoutPtr_ = new QVBoxLayout(leftPanelPtr_);
    // companionPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    // companionPanelLayoutPtr_->setSpacing(0);
    // companionPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // companionPanelPtr_->setLayout(companionPanelLayoutPtr_);

    // leftPanelLayoutPtr_->addWidget(companionPanelPtr_);

    // add stub widget to companion panel
    // SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    // SocketInfoBaseWidget* baseObjectCastPtr =
    //     dynamic_cast<SocketInfoBaseWidget*>(stub);

    // companionPanelLayoutPtr_->addWidget(baseObjectCastPtr);

    // companionPanelPtr_->resize(4000, 1000);  // TODO ???

    // spacerPtr_ = new QSpacerItem(
    //     0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    // leftPanelLayoutPtr_->addSpacerItem(spacerPtr_);

    // showHideWidgetPtr_ = new ShowHideWidget;
    // leftPanelLayoutPtr_->addWidget(showHideWidgetPtr_);

    // central panel

    centralPanelPtr_ = new CentralPanelWidget(centralWidgetPtr_);

    // centralPanelPtr_ = new QWidget(centralWidgetPtr_);
    // // centralPanelPtr_->setStyleSheet("border-right: 1px solid black");
    // centralPanelLayoutPtr_ = new QVBoxLayout;
    // centralPanelLayoutPtr_->setSpacing(0);
    // centralPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // centralPanelPtr_->setLayout(centralPanelLayoutPtr_);

    // companionNameLabelPtr_ = new QLabel("", centralPanelPtr_);
    // // companionNameLabelPtr_->setStyleSheet("border-bottom: 1px solid black");
    // companionNameLabelPalettePtr_ = new QPalette;
    // companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(companionNameLabelBackgroundColor));
    // companionNameLabelPtr_->setAutoFillBackground(true);
    // companionNameLabelPtr_->setPalette(*companionNameLabelPalettePtr_);
    // //    companionNameLabel_->hide();

    // centralPanelLayoutPtr_->addWidget(companionNameLabelPtr_);

    // chatHistoryWidgetStubPtr_ = new QPlainTextEdit(centralPanelPtr_);
    // chatHistoryWidgetStubPtr_->setReadOnly(true);
    // chatHistoryWidgetStubPtr_->setPlainText("");

    // chatHistoryWidgetStubPalettePtr_ = new QPalette;
    // chatHistoryWidgetStubPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    // //    chatHistoryWidgetStubPalette_->setColor(QPalette::Text, QColorConstants::Black);
    // //    chatHistoryWidgetStub_->setAutoFillBackground(true);
    // chatHistoryWidgetStubPtr_->setPalette(*chatHistoryWidgetStubPalettePtr_);

    // centralPanelLayoutPtr_->addWidget(chatHistoryWidgetStubPtr_);

    // textEditStubPtr_ = new TextEditWidget;
    // // textEditStubPalettePtr_ = new QPalette;  // TODO move to widget ctor
    // // textEditStubPalettePtr_->setColor(QPalette::Base, QColor(0xdadada));
    // // textEditStubPtr_->setAutoFillBackground(true);
    // // textEditStubPtr_->setPalette(*textEditStubPalettePtr_);
    // centralPanelLayoutPtr_->addWidget(textEditStubPtr_);
    // //    textEdit_->hide();

    centralWidgetLayoutPtr_->addWidget(centralPanelPtr_);

    // right panel

    rightPanelPtr_ = new RightPanelWidget(centralWidgetPtr_);

    // rightPanelPtr_ = new QWidget(centralWidgetPtr_);
    // // rightPanelPtr_->setStyleSheet("border-right: 1px solid black");
    // rightPanelLayoutPtr_ = new QVBoxLayout;
    // rightPanelLayoutPtr_->setSpacing(0);
    // rightPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // rightPanelPtr_->setLayout(rightPanelLayoutPtr_);

    // appLogWidgetPtr_ = new QPlainTextEdit(centralWidgetPtr_);
    // // appLogWidgetPtr_->setStyleSheet("border-left: 1px solid black");
    // appLogWidgetPtr_->setReadOnly(true);
    // appLogWidgetPtr_->setPlainText("");

    // appLogWidgetPalettePtr_ = new QPalette;
    // appLogWidgetPalettePtr_->setColor(QPalette::Base, QColor(appLogBackgroundColor));
    // appLogWidgetPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    // appLogWidgetPtr_->setAutoFillBackground(true);
    // appLogWidgetPtr_->setPalette(*appLogWidgetPalettePtr_);

    // rightPanelLayoutPtr_->addWidget(appLogWidgetPtr_);

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    centralWidgetLayoutPtr_->addWidget(rightPanelPtr_);

    //    selectedSocketInfoWidget_ = nullptr;
    // selectedCompanion_ = nullptr;

    // logging enabled, actions
    // setLeftPanel();
    //    addTestSocketInfoWidgetToLeftPanel();
}

MainWindow::~MainWindow()
{
    // cannot set parent for palette
    delete this->menuBarPalettePtr_;
    // delete this->companionNameLabelPalettePtr_;
    // delete this->chatHistoryWidgetStubPalettePtr_;
    // delete this->textEditStubPalettePtr_;
    // delete this->appLogWidgetPalettePtr_;
}

void MainWindow::setCompanionPanel()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    if(companionPanelChildren.size() == 0)
    {
        this->addStubWidgetToCompanionPanel();
    }
    else
    {
        logArgsWarning(
            "companionPanelChildren.size() != 0 "
            "at MainWindow::setCompanionPanel()");
    }
}

void MainWindow::set()
{
    this->createMenu();
    this->setCompanionPanel();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::exit(0);
}

// void MainWindow::addTextToChatHistoryWidget(const QString& text)
// {
//     this->chatHistoryWidgetStubPtr_->appendPlainText(text);
//     QApplication::processEvents();
// }

void MainWindow::addTextToAppLogWidget(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addWidgetToCompanionPanel(SocketInfoBaseWidget* widget)
{
    this->leftPanelWidgetPtr_->addWidgetToCompanionPanel(widget);
}

// void MainWindow::addWidgetToCentralPanel(QWidget* widget)
// {
//     this->centralPanelLayoutPtr_->addWidget(widget);
// }

void MainWindow::setCentralPanel(CentralPanelWidget* widget)
{
    this->centralPanelWidgetPtr_ = widget;
}

void MainWindow::setRightPanel(RightPanelWidget* widget)
{
    this->rightPanelWidgetPtr_ = widget;
}

size_t MainWindow::getCompanionPanelChildrenSize()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    return companionPanelChildren.size();
}

void MainWindow::removeStubsFromCompanionPanel()
{
    QList<SocketInfoBaseWidget*> companionPanelChildren =
        this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
            Qt::FindDirectChildrenOnly);

    for(auto& child : companionPanelChildren)
    {
        if(child->isStub())
        {
            this->companionPanelLayoutPtr_->removeWidget(child);
            delete child;
        }
    }
}

void MainWindow::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)
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

void MainWindow::oldSelectedCompanionActions(const Companion* companion)
{
    if(companion)
    {
        this->companionNameLabelPtr_->setText("");
        //        this->companionNameLabel_->hide();

        this->chatHistoryWidgetStubPtr_->setPlainText("");
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->hide();
        this->textEditStubPtr_->hide();
    }
}

void MainWindow::newSelectedCompanionActions(const Companion* companion)
{
    if(companion)
    {
        this->companionNameLabelPtr_->setText(
            QString::fromStdString(companion->getName()));
        this->companionNameLabelPtr_->show();
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->setPlainText("");
        this->chatHistoryWidgetStubPtr_->show();

        this->textEditStubPtr_->setText("");
        this->textEditStubPtr_->show();
    }
}

void MainWindow::createCompanion()
{
    getGraphicManagerPtr()->createCompanion();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        // this->resetSelectedCompanion(nullptr);
        // Manager* managerPtr = getManagerPtr();
        // managerPtr->resetSelectedCompanion(nullptr);
        getManagerPtr()->resetSelectedCompanion(nullptr);
    }
}

void MainWindow::createMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, &QCoreApplication::quit);

    QMenu* companionMenu = menuBar()->addMenu("Companion");
    QAction* addCompanionAction = new QAction("Add new companion", this);
    companionMenu->addAction(addCompanionAction);
    connect(addCompanionAction, &QAction::triggered, this, &MainWindow::createCompanion);
}
