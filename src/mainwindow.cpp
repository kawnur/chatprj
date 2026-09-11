#include "mainwindow.hpp"

#include <QMenuBar>

#include "graphic_manager.hpp"
#include "manager.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_panel.hpp"

MainWindow::MainWindow()
{
    setWindowTitle(std::getenv("CLIENT_NAME"));

    // menu bar
    menuBarPalette_ = std::make_unique<QPalette>();
    menuBarPalette_->setColor(QPalette::Window, QColor(mainWindowMenuBarBackgroundColor));

    auto *bar = menuBar();
    bar->setAutoFillBackground(true);
    bar->setPalette(*menuBarPalette_);

    // central widget
    centralWidget_ = std::shared_ptr<QWidget>();
    setCentralWidget(centralWidget_.get());

    centralWidgetLayout_ = std::make_unique<QHBoxLayout>(centralWidget_.get());
    centralWidgetLayout_->setSpacing(0);
    centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    centralWidget_->setLayout(centralWidgetLayout_.get());

    // left panel
    leftContainerWidget_ = std::make_shared<MainWindowContainerWidget>(centralWidget_);
    centralWidgetLayout_->addWidget(leftContainerWidget_.get());

    leftPanel_ = std::make_shared<LeftPanelWidget>(leftContainerWidget_);
    leftContainerWidget_->addWidgetToLayout(dynamic_pointer_cast<QWidget>(leftPanel_));

    // central panel
    centralContainerWidget_ = std::make_shared<MainWindowContainerWidget>(centralWidget_);
    centralWidgetLayout_->addWidget(centralContainerWidget_.get());

    centralPanel_ = nullptr;

    // right panel
    rightContainerWidget_ = std::make_shared<MainWindowContainerWidget>(centralWidget_);
    centralWidgetLayout_->addWidget(rightContainerWidget_.get());

    rightPanel_ = std::make_shared<RightPanelWidget>(rightContainerWidget_);
    rightPanel_->set();

    rightContainerWidget_->addWidgetToLayout(rightPanel_);

    // map container ptr to position
    containerMap[MainWindowContainerPosition::LEFT] = leftContainerWidget_;
    containerMap[MainWindowContainerPosition::CENTRAL] = centralContainerWidget_;
    containerMap[MainWindowContainerPosition::RIGHT] = rightContainerWidget_;
    
    // blur effect moved to set

    // splitter
    splitter_ = std::make_unique<QSplitter>(centralWidget_.get());
    // splitter_-addWidget(leftContainerWidget_);
    splitter_->addWidget(centralContainerWidget_.get());
    splitter_->addWidget(rightContainerWidget_.get());
    centralWidgetLayout_->addWidget(splitter_.get());
}

void MainWindow::set()
{
    createMenu();

    auto graphicManager = getGraphicManager();
    graphicManager->setParentsForStubs(leftContainerWidget_, centralContainerWidget_);
    graphicManager->setStubWidgets();
    graphicManager->showCentralPanelStub();

    showHideWidget_ = std::make_shared<ShowHideWidget>();
    addWidgetToContainerAndSetParentTo(MainWindowContainerPosition::LEFT, showHideWidget_);

    setBlurEffect();
}

void MainWindow::addTextToAppLogWidget(const QString &text)
{
    rightPanel_->addTextToAppLogWidget(text);
}

void MainWindow::addWidgetToContainerAndSetParentTo(
    MainWindowContainerPosition position, std::shared_ptr<QWidget> widget)
{
    auto lambda = [&]()
    {
        containerMap.at(position)->addWidgetToLayoutAndSetParentTo(widget);
    };

    runAndLogException(lambda);
}

void MainWindow::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
{
    leftPanel_->addWidgetToCompanionPanel(widget);
}

std::size_t MainWindow::getCompanionPanelChildrenSize()
{
    return leftPanel_->getCompanionPanelChildrenSize();
}

void MainWindow::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
{
    leftPanel_->removeWidgetFromCompanionPanel(widget);
}

void MainWindow::hideLeftAndRightPanels()
{
    leftPanel_->hide();
    rightPanel_->hide();
}

void MainWindow::showLeftAndRightPanels()
{
    leftPanel_->show();
    rightPanel_->show();
}

int MainWindow::getLeftPanelWidgetWidth()
{
    return leftPanel_->getLastCompanionPanelChildWidth();
}

void MainWindow::enableWidgetsForShowHide()
{
    menuBar()->setEnabled(true);
    leftPanel_->setEnabled(true);
    centralContainerWidget_->setEnabled(true);
    rightContainerWidget_->setEnabled(true);
}

void MainWindow::disableWidgetsForShowHide()
{
    menuBar()->setEnabled(false);
    leftPanel_->setEnabled(false);
    centralContainerWidget_->setEnabled(false);
    rightContainerWidget_->setEnabled(false);
}

void MainWindow::enableBlurEffect()
{
    disableWidgetsForShowHide();
    setGraphicsEffect(blurEffect_.get());
}

void MainWindow::disableBlurEffect()
{
    enableWidgetsForShowHide();
    setGraphicsEffect(nullptr);

    // // setGraphicsEffect deletes previous effect object
    setBlurEffect();
}

void MainWindow::createCompanion()
{
    getGraphicManager()->createCompanion();
}

void MainWindow::createGroupChat()
{
    getGraphicManager()->createGroupChat();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        getManager()->resetSelectedCompanion(nullptr);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    // showFullScreen();
}

void MainWindow::createMenu()
{
    auto *bar = menuBar();

    QMenu *fileMenu = bar->addMenu("File");
    auto exitAction = std::make_shared<QAction>("Exit", this);
    fileMenu->addAction(exitAction.get());

    connect(
        exitAction.get(), &QAction::triggered, this, &QCoreApplication::quit, Qt::QueuedConnection);

    QMenu *companionMenu = bar->addMenu("Companion");
    auto addCompanionAction = std::make_shared<QAction>("Add new companion", this);
    companionMenu->addAction(addCompanionAction.get());

    connect(
        addCompanionAction.get(), &QAction::triggered, this, &MainWindow::createCompanion,
        Qt::QueuedConnection);

    QMenu *groupChatMenu = bar->addMenu("Group chat");
    auto addGroupChatAction = std::make_shared<QAction>("Add new group chat", this);
    groupChatMenu->addAction(addGroupChatAction.get());

    connect(
        addGroupChatAction.get(), &QAction::triggered, this, &MainWindow::createGroupChat,
        Qt::QueuedConnection);
}

void MainWindow::setBlurEffect()
{
    blurEffect_ = std::make_shared<QGraphicsBlurEffect>();
    blurEffect_->setBlurRadius(30);
}
