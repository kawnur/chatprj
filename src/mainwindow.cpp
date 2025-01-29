#include "mainwindow.hpp"

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
    leftContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(leftContainerWidgetPtr_);

    leftPanelPtr_ = new LeftPanelWidget(leftContainerWidgetPtr_);
    leftContainerWidgetPtr_->addWidgetToLayout(leftPanelPtr_);

    // central panel
    centralContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(centralContainerWidgetPtr_);

    centralPanelPtr_ = nullptr;

    // right panel
    rightContainerWidgetPtr_ = new MainWindowContainerWidget(centralWidgetPtr_);
    centralWidgetLayoutPtr_->addWidget(rightContainerWidgetPtr_);

    rightPanelPtr_ = new RightPanelWidget(rightContainerWidgetPtr_);
    rightPanelPtr_->set();

    rightContainerWidgetPtr_->addWidgetToLayout(rightPanelPtr_);

    // map container ptr to position
    mapContainerPtrToContainerPosition[MainWindowContainerPosition::LEFT] =
        leftContainerWidgetPtr_;

    mapContainerPtrToContainerPosition[MainWindowContainerPosition::CENTRAL] =
        centralContainerWidgetPtr_;

    mapContainerPtrToContainerPosition[MainWindowContainerPosition::RIGHT] =
        rightContainerWidgetPtr_;
    
    // blur effect moved to set
}

MainWindow::~MainWindow()
{
    // cannot set parent for palette
    delete this->menuBarPalettePtr_;
    delete this->centralWidgetPtr_;
    delete this->blurEffectPtr_;
}

void MainWindow::set()
{
    this->createMenu();

    getGraphicManagerPtr()->setParentsForStubs(
        this->leftContainerWidgetPtr_, this->centralContainerWidgetPtr_);

    getGraphicManagerPtr()->setStubWidgets();

    getGraphicManagerPtr()->showCentralPanelStub();

    this->showHideWidgetPtr_ = new ShowHideWidget;

    this->addWidgetToContainerAndSetParentTo(
        MainWindowContainerPosition::LEFT, this->showHideWidgetPtr_);

    this->setBlurEffect();
}

void MainWindow::addTextToAppLogWidget(const QString& text)
{
    this->rightPanelPtr_->addTextToAppLogWidget(text);
    // QApplication::processEvents();
}

void MainWindow::addWidgetToContainerAndSetParentTo(
    MainWindowContainerPosition position, QWidget* widgetPtr)
{
    this->mapContainerPtrToContainerPosition.at(position)->
        addWidgetToLayoutAndSetParentTo(widgetPtr);
}

void MainWindow::addWidgetToCompanionPanel(SocketInfoBaseWidget* widgetPtr)
{
    this->leftPanelPtr_->addWidgetToCompanionPanel(widgetPtr);
}

size_t MainWindow::getCompanionPanelChildrenSize()
{
    return this->leftPanelPtr_->getCompanionPanelChildrenSize();
}

void MainWindow::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widgetPtr)
{
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

int MainWindow::getLeftPanelWidgetWidth()
{
    return this->leftPanelPtr_->getLastCompanionPanelChildWidth();
}

void MainWindow::enableWidgetsForShowHide()
{
    this->menuBar()->setEnabled(true);
    this->leftPanelPtr_->setEnabled(true);
    this->centralContainerWidgetPtr_->setEnabled(true);
    this->rightContainerWidgetPtr_->setEnabled(true);
}

void MainWindow::disableWidgetsForShowHide()
{
    this->menuBar()->setEnabled(false);
    this->leftPanelPtr_->setEnabled(false);
    this->centralContainerWidgetPtr_->setEnabled(false);
    this->rightContainerWidgetPtr_->setEnabled(false);
}

void MainWindow::enableBlurEffect()
{
    this->disableWidgetsForShowHide();
    this->setGraphicsEffect(this->blurEffectPtr_);
}

void MainWindow::disableBlurEffect()
{
    this->enableWidgetsForShowHide();
    this->setGraphicsEffect(nullptr);

    // // setGraphicsEffect deletes previous effect object
    this->setBlurEffect();
}

void MainWindow::createCompanion()
{
    getGraphicManagerPtr()->createCompanion();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        getManagerPtr()->resetSelectedCompanion(nullptr);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    // this->showFullScreen();
}

void MainWindow::createMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("File");
    QAction* exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);

    connect(
        exitAction, &QAction::triggered,
        this, &QCoreApplication::quit, Qt::QueuedConnection);

    QMenu* companionMenu = menuBar()->addMenu("Companion");
    QAction* addCompanionAction = new QAction("Add new companion", this);
    companionMenu->addAction(addCompanionAction);

    connect(
        addCompanionAction, &QAction::triggered,
        this, &MainWindow::createCompanion, Qt::QueuedConnection);
}

void MainWindow::setBlurEffect()
{
    blurEffectPtr_ = new QGraphicsBlurEffect;
    blurEffectPtr_->setBlurRadius(30);
}
