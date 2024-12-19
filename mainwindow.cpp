#include "mainwindow.hpp"

// MainWindow* getMainWindowPtr()
// {
//     QCoreApplication* coreApp = QCoreApplication::instance();
//     ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
//     return app->mainWindowPtr_;
// }

// void MainWindow::addStubWidgetToCompanionPanel()
// {
//     SocketInfoStubWidget* stub = new SocketInfoStubWidget;

//     SocketInfoBaseWidget* baseObjectCastPtr =
//             dynamic_cast<SocketInfoBaseWidget*>(stub);

//     this->companionPanelLayoutPtr_->addWidget(baseObjectCastPtr);
// }

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

    centralWidgetLayoutPtr_ = new QHBoxLayout(centralWidgetPtr_);
    centralWidgetLayoutPtr_->setSpacing(0);
    centralWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    centralWidgetPtr_->setLayout(centralWidgetLayoutPtr_);

    // left panel
    // leftContainerWidgetPtr_ = new QWidget(centralWidgetPtr_);
    // leftContainerWidgetLayoutPtr_ = new QVBoxLayout(leftContainerWidgetPtr_);
    // leftContainerWidgetLayoutPtr_->setSpacing(0);
    // leftContainerWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // leftContainerWidgetPtr_->setLayout(leftContainerWidgetLayoutPtr_);

    leftContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(leftContainerWidgetPtr_);

    leftPanelPtr_ = new LeftPanelWidget(leftContainerWidgetPtr_);
    leftContainerWidgetPtr_->addWidgetToLayout(leftPanelPtr_);

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

    // centralWidgetLayoutPtr_->addWidget(leftPanelPtr_);

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

    // centralPanelPtr_ = new CentralPanelWidget(centralWidgetPtr_, "");
    // centralContainerWidgetPtr_ = new QWidget(centralWidgetPtr_);
    // centralContainerWidgetLayoutPtr_ = new QVBoxLayout(centralContainerWidgetPtr_);
    // centralContainerWidgetLayoutPtr_->setSpacing(0);
    // centralContainerWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // centralContainerWidgetPtr_->setLayout(centralContainerWidgetLayoutPtr_);

    centralContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(centralContainerWidgetPtr_);

    centralPanelPtr_ = nullptr;

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

    // centralWidgetLayoutPtr_->addWidget(centralPanelPtr_);

    // right panel
    // rightContainerWidgetPtr_= new QWidget(centralWidgetPtr_);
    // rightContainerWidgetLayoutPtr_ = new QVBoxLayout(rightContainerWidgetPtr_);
    // rightContainerWidgetLayoutPtr_->setSpacing(0);
    // rightContainerWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    // rightContainerWidgetPtr_->setLayout(rightContainerWidgetLayoutPtr_);

    rightContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(rightContainerWidgetPtr_);

    rightPanelPtr_ = new RightPanelWidget(rightContainerWidgetPtr_);
    rightPanelPtr_->set();

    rightContainerWidgetPtr_->addWidgetToLayout(rightPanelPtr_);

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

    // centralWidgetLayoutPtr_->addWidget(rightPanelPtr_);

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

// void MainWindow::setCompanionPanel()
// {
//     QList<SocketInfoBaseWidget*> companionPanelChildren =
//         this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
//             Qt::FindDirectChildrenOnly);

//     if(companionPanelChildren.size() == 0)
//     {
//         this->addStubWidgetToCompanionPanel();
//     }
//     else
//     {
//         logArgsWarning(
//             "companionPanelChildren.size() != 0 "
//             "at MainWindow::setCompanionPanel()");
//     }
// }

void MainWindow::set()
{
    this->createMenu();
    // this->setCompanionPanel();

    getGraphicManagerPtr()->setParentsForStubs(
        this->leftContainerWidgetPtr_, this->centralContainerWidgetPtr_);

    getGraphicManagerPtr()->setStubWidgets();

    getGraphicManagerPtr()->showCentralPanelStub();

    this->showHideWidgetPtr_ = new ShowHideWidget;
    this->addWidgetToLeftContainerAndSetParentTo(showHideWidgetPtr_);
}

void MainWindow::addLeftPanelToLayout()
{
    this->centralWidgetLayoutPtr_->addWidget(this->leftPanelPtr_);
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
    // this->appLogWidgetPtr_->appendPlainText(text);
    this->rightPanelPtr_->addTextToAppLogWidget(text);
    QApplication::processEvents();
}

void MainWindow::addWidgetToLeftContainerAndSetParentTo(QWidget* widgetPtr)
{
    this->leftContainerWidgetPtr_->addWidgetToLayoutAndSetParentTo(widgetPtr);
}

void MainWindow::addWidgetToCentralContainerAndSetParentTo(QWidget* widgetPtr)
{
    this->centralContainerWidgetPtr_->addWidgetToLayoutAndSetParentTo(widgetPtr);
}

void MainWindow::addWidgetToRightContainerAndSetParentTo(QWidget* widgetPtr)
{
    this->rightContainerWidgetPtr_->addWidgetToLayoutAndSetParentTo(widgetPtr);
}

// void MainWindow::addWidgetToCentralWidgetLayout(QWidget* widgetPtr)
// {
//     this->centralWidgetLayoutPtr_->addWidget(widgetPtr);
// }

void MainWindow::addWidgetToCompanionPanel(SocketInfoBaseWidget* widgetPtr)
{
    // this->leftPanelWidgetPtr_->addWidgetToCompanionPanel(widget);
    this->leftPanelPtr_->addWidgetToCompanionPanel(widgetPtr);
}

// void MainWindow::addWidgetToCentralPanel(QWidget* widget)
// {
//     this->centralPanelLayoutPtr_->addWidget(widget);
// }

void MainWindow::setLeftPanel(LeftPanelWidget* widgetPtr)
{
    this->leftPanelPtr_ = widgetPtr;
}

void MainWindow::setCentralPanel(CentralPanelWidget* widgetPtr)
{
    // this->centralPanelWidgetPtr_ = widget;
    this->centralPanelPtr_ = widgetPtr;
}

void MainWindow::setRightPanel(RightPanelWidget* widgetPtr)
{
    // this->rightPanelWidgetPtr_ = widget;
    this->rightPanelPtr_ = widgetPtr;
}

size_t MainWindow::getCompanionPanelChildrenSize()
{
    // QList<SocketInfoBaseWidget*> companionPanelChildren =
    //     this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
    //         Qt::FindDirectChildrenOnly);

    // return companionPanelChildren.size();
    return this->leftPanelPtr_->getCompanionPanelChildrenSize();
}

void MainWindow::removeStubsFromCompanionPanel()  // TODO do we need remove?
{
    // QList<SocketInfoBaseWidget*> companionPanelChildren =
    //     this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
    //         Qt::FindDirectChildrenOnly);

    // for(auto& child : companionPanelChildren)
    // {
    //     if(child->isStub())
    //     {
    //         this->companionPanelLayoutPtr_->removeWidget(child);
    //         delete child;
    //     }
    // }
    this->leftPanelPtr_->removeStubsFromCompanionPanel();
}

void MainWindow::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)  // TODO do we nedd remove?
{
    // QList<SocketInfoBaseWidget*> companionPanelChildren =
    //     this->companionPanelPtr_->findChildren<SocketInfoBaseWidget*>(
    //         Qt::FindDirectChildrenOnly);

    // qsizetype index = companionPanelChildren.indexOf(widgetPtr);

    // if(index == -1)
    // {
    //     showErrorDialogAndLogError("SocketInfoBaseWidget was not found in companion panel");
    // }
    // else if(index == 0)
    // {

    // }
    // else if(index > 0)
    // {
    //     Manager* managerPtr = getManagerPtr();

    //     auto previousWidget = companionPanelChildren.at(index - 1);

    //     auto previousCompanionPtr = managerPtr->
    //         getMappedCompanionBySocketInfoBaseWidget(previousWidget);

    //     managerPtr->resetSelectedCompanion(previousCompanionPtr);

    //     this->companionPanelLayoutPtr_->removeWidget(widgetPtr);
    // }
    this->leftPanelPtr_->removeWidgetFromCompanionPanel(widgetPtr);
}

void MainWindow::hideLeftAndRightPanels()
{
    this->leftPanelPtr_->hide();
    this->rightPanelPtr_->hide();
}

void MainWindow::showLeftAndRightPanels()
{
    this->leftPanelPtr_->show();
    this->rightPanelPtr_->show();
}

// void MainWindow::oldSelectedCompanionActions(const Companion* companion)
// {
//     // if(companion)
//     // {
//     //     this->companionNameLabelPtr_->setText("");
//     //     //        this->companionNameLabel_->hide();

//     //     this->chatHistoryWidgetStubPtr_->setPlainText("");
//     // }
//     // else
//     // {
//     //     this->chatHistoryWidgetStubPtr_->hide();
//     //     this->textEditStubPtr_->hide();
//     // }
//     this->centralPanelPtr_->oldSelectedCompanionActions(companion);
// }

// void MainWindow::newSelectedCompanionActions(const Companion* companion)
// {
//     // if(companion)
//     // {
//     //     this->companionNameLabelPtr_->setText(
//     //         QString::fromStdString(companion->getName()));
//     //     this->companionNameLabelPtr_->show();
//     // }
//     // else
//     // {
//     //     this->chatHistoryWidgetStubPtr_->setPlainText("");
//     //     this->chatHistoryWidgetStubPtr_->show();

//     //     this->textEditStubPtr_->setText("");
//     //     this->textEditStubPtr_->show();
//     // }
//     this->centralPanelPtr_->newSelectedCompanionActions(companion);
// }

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
