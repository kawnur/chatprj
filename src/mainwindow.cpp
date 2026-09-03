#include "mainwindow.hpp"

MainWindow::MainWindow() {
    setWindowTitle(std::getenv("CLIENT_NAME"));

    // menu bar
    menuBarPalette_ = new QPalette;

    menuBarPalette_->setColor(
        QPalette::Window, QColor(mainWindowMenuBarBackgroundColor));

    menuBar()->setAutoFillBackground(true);
    menuBar()->setPalette(*menuBarPalette_);

    // central widget
    centralWidget_ = new QWidget;
    setCentralWidget(centralWidget_);

    centralWidgetLayout_ = new QHBoxLayout(centralWidget_);
    centralWidgetLayout_->setSpacing(0);
    centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    centralWidget_->setLayout(centralWidgetLayout_);

    // left panel
    leftContainerWidget_ = new MainWindowContainerWidget(centralWidget_);
    centralWidgetLayout_->addWidget(leftContainerWidget_);

    leftPanel_ = new LeftPanelWidget(leftContainerWidget_);
    leftContainerWidget_->addWidgetToLayout(leftPanel_);

    // central panel
    centralContainerWidget_ = new MainWindowContainerWidget(centralWidget_);
    centralWidgetLayout_->addWidget(centralContainerWidget_);

    centralPanel_ = nullptr;

    // right panel
    rightContainerWidget_ = new MainWindowContainerWidget(centralWidget_);
    centralWidgetLayout_->addWidget(rightContainerWidget_);

    rightPanel_ = new RightPanelWidget(rightContainerWidget_);
    rightPanel_->set();

    rightContainerWidget_->addWidgetToLayout(rightPanel_);

    // map container ptr to position
    mapContainerToContainerPosition[MainWindowContainerPosition::LEFT] =
        leftContainerWidget_;

    mapContainerToContainerPosition[MainWindowContainerPosition::CENTRAL] =
        centralContainerWidget_;

    mapContainerToContainerPosition[MainWindowContainerPosition::RIGHT] =
        rightContainerWidget_;
    
    // blur effect moved to set

    // splitter
    splitter_ = new QSplitter(centralWidget_);
    // splitter_->addWidget(leftContainerWidget_);
    splitter_->addWidget(centralContainerWidget_);
    splitter_->addWidget(rightContainerWidget_);
    centralWidgetLayout_->addWidget(splitter_);
}

void MainWindow::set() {
    this->createMenu();

    getGraphicManager()->setParentsForStubs(
        this->leftContainerWidget_, this->centralContainerWidget_);

    getGraphicManager()->setStubWidgets();

    getGraphicManager()->showCentralPanelStub();

    this->showHideWidget_ = new ShowHideWidget;

    this->addWidgetToContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, this->showHideWidget_);

    this->setBlurEffect();
}

void MainWindow::addTextToAppLogWidget(const QString& text) {
    this->rightPanel_->addTextToAppLogWidget(text);    
}

void MainWindow::addWidgetToContainerAndSetParentTo(
    MainWindowContainerPosition position, std::shared_ptr<QWidget> widget) {
    this->mapContainerToContainerPosition.at(position)->
        addWidgetToLayoutAndSetParentTo(widget);
}

void MainWindow::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->leftPanel_->addWidgetToCompanionPanel(widget);
}

std::size_t MainWindow::getCompanionPanelChildrenSize() {
    return this->leftPanel_->getCompanionPanelChildrenSize();
}

void MainWindow::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->leftPanel_->removeWidgetFromCompanionPanel(widget);
}

void MainWindow::hideLeftAndRightPanels() {
    this->leftPanel_->hide();
    this->rightPanel_->hide();
}

void MainWindow::showLeftAndRightPanels() {
    this->leftPanel_->show();
    this->rightPanel_->show();
}

int MainWindow::getLeftPanelWidgetWidth() {
    return this->leftPanel_->getLastCompanionPanelChildWidth();
}

void MainWindow::enableWidgetsForShowHide() {
    this->menuBar()->setEnabled(true);
    this->leftPanel_->setEnabled(true);
    this->centralContainerWidget_->setEnabled(true);
    this->rightContainerWidget_->setEnabled(true);
}

void MainWindow::disableWidgetsForShowHide() {
    this->menuBar()->setEnabled(false);
    this->leftPanel_->setEnabled(false);
    this->centralContainerWidget_->setEnabled(false);
    this->rightContainerWidget_->setEnabled(false);
}

void MainWindow::enableBlurEffect() {
    this->disableWidgetsForShowHide();
    this->setGraphicsEffect(this->blurEffect_);
}

void MainWindow::disableBlurEffect() {
    this->enableWidgetsForShowHide();
    this->setGraphicsEffect(nullptr);

    // // setGraphicsEffect deletes previous effect object
    this->setBlurEffect();
}

void MainWindow::createCompanion() {
    getGraphicManager()->createCompanion();
}

void MainWindow::createGroupChat() {
    getGraphicManager()->createGroupChat();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    std::exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        getManager()->resetSelectedCompanion(nullptr);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    // this->showFullScreen();
}

void MainWindow::createMenu() {
    std::shared_ptr<QMenu> fileMenu = menuBar()->addMenu("File");
    std::shared_ptr<QAction> exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);

    connect(
        exitAction, &QAction::triggered,
        this, &QCoreApplication::quit, Qt::QueuedConnection);

    std::shared_ptr<QMenu> companionMenu = menuBar()->addMenu("Companion");
    std::shared_ptr<QAction> addCompanionAction = new QAction("Add new companion", this);
    companionMenu->addAction(addCompanionAction);

    connect(
        addCompanionAction, &QAction::triggered,
        this, &MainWindow::createCompanion, Qt::QueuedConnection);

    std::shared_ptr<QMenu> groupChatMenu = menuBar()->addMenu("Group chat");
    std::shared_ptr<QAction> addGroupChatAction = new QAction("Add new group chat", this);
    groupChatMenu->addAction(addGroupChatAction);

    connect(
        addGroupChatAction, &QAction::triggered,
        this, &MainWindow::createGroupChat, Qt::QueuedConnection);
}

void MainWindow::setBlurEffect() {
    blurEffect_ = new QGraphicsBlurEffect;
    blurEffect_->setBlurRadius(30);
}
