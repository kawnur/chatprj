#include "mainwindow.hpp"

#include <QMenuBar>

#include "graphic_manager.hpp"
#include "utils_widgets.hpp"
#include "widgets_panel.hpp"

MainWindow::MainWindow()
{
    setWindowTitle(std::getenv("CLIENT_NAME"));

    // menu bar
    menuBarPalette_ = std::make_unique<QPalette>();
    menuBarPalette_->setColor(QPalette::Window, QColor(getWidgetColor(WidgetType::MAIN_WINDOW_MENU)));

    auto *bar = menuBar();
    bar->setAutoFillBackground(true);
    bar->setPalette(*menuBarPalette_);

    // central widget
    mainWidget_ = std::make_unique<MainWidget>();
    mainWidget_->set();
    setCentralWidget(mainWidget_.get());
}

void MainWindow::set()
{
    createMenu();
}

void MainWindow::addLog(const QString &text)
{
    mainWidget_->addLog(text);
}

bool MainWindow::addWidget(MWContainerPosition position, QWidgetPtr widget)
{
    return mainWidget_->addWidget(position, widget);
}

void MainWindow::addCompanion(SocketInfoBaseWidgetPtr widget)
{
    mainWidget_->addCompanion(widget);
}

void MainWindow::removeCompanion(SocketInfoBaseWidgetPtr widget)
{
    mainWidget_->removeCompanion(widget);
}

std::size_t MainWindow::getCompanionPanelChildrenSize()
{
    return mainWidget_->getCompanionPanelChildrenSize();
}

void MainWindow::enable()
{
    menuBar()->setEnabled(true);
    mainWidget_->enable();
}

void MainWindow::disable()
{
    menuBar()->setEnabled(false);
    mainWidget_->disable();
}

void MainWindow::enableBlur()
{
    disable();
    mainWidget_->setBlur();
}

void MainWindow::disableBlur()
{
    enable();
    mainWidget_->initBlurEffect();
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
        getGraphicManager()->resetSelectedCompanion(nullptr);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    // showFullScreen();
}

void MainWindow::createMenu()
{
    auto *bar = menuBar();

    QMenu* fileMenu = bar->addMenu("File");
    QAction* exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);

    connect(
        exitAction, &QAction::triggered,
        this, &QCoreApplication::quit, Qt::QueuedConnection);

    QMenu* companionMenu = bar->addMenu("Companion");
    QAction* addCompanionAction = new QAction("Add new companion", this);
    companionMenu->addAction(addCompanionAction);

    connect(
        addCompanionAction, &QAction::triggered,
        this, &MainWindow::createCompanion, Qt::QueuedConnection);

    QMenu* groupChatMenu = bar->addMenu("Group chat");
    QAction* addGroupChatAction = new QAction("Add new group chat", this);
    groupChatMenu->addAction(addGroupChatAction);

    connect(
        addGroupChatAction, &QAction::triggered,
        this, &MainWindow::createGroupChat, Qt::QueuedConnection);
}

void MainWindow::hideCentralStub()
{
    mainWidget_->hideCentralStub();
}

void MainWindow::showCentralStub()
{
    mainWidget_->showCentralStub();
}

void MainWindow::hideCompanionStub()
{
    mainWidget_->hideCompanionStub();
}

void MainWindow::showStubs()
{
    mainWidget_->showStubs();
}

void MainWindow::hideStubs()
{
    mainWidget_->hideStubs();
}
