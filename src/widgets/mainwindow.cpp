#include "mainwindow.hpp"

#include <QMenuBar>

#include "graphic_manager.hpp"
#include "utils.hpp"
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

void MainWindow::addWidgetToCentralContainer(QWidgetPtr widget)
{
    mainWidget_->addWidgetToCentralContainer(widget);
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
    std::vector<std::pair<const QString &, std::vector<MenuItemInfo>>> data {
        {
            "File",
            {
                { { "Exit" }, [this]() { QCoreApplication::quit(); } },
            }
        },
        {
            "Companion",
            {
                { { "Add new companion" }, [this]() { createCompanion(); } },
            }
        },
        {
            "Group chat",
            {
                { { "Add new group chat" }, [this]() { createGroupChat(); } },
            }
        }
    };

    auto *bar = menuBar();

    // auto lambda = [&](const QString &name, const std::vector<MenuItemInfo> &info)
    auto lambda = [&](QMenu* menu, const std::vector<MenuItemInfo> &info)
    {
        // QMenu* menu = bar->addMenu(name);

        for (const auto &item : info) {
            auto text = getIndexedContainerValue(item.texts(), 0, "");

            QAction* action = new QAction(text, this);
            menu->addAction(action);

            connect(action, &QAction::triggered, this, item.function(), Qt::QueuedConnection);
        }
    };

    // TODO get rid of notifier crash
    // for (const auto &pair : data)
    //     lambda(pair.first, pair.second);

    QMenu* fileMenu = bar->addMenu("File");
    QMenu* companionMenu = bar->addMenu("Companion");
    QMenu* groupChatMenu = bar->addMenu("Group chat");

    lambda(fileMenu, data[0].second);
    lambda(companionMenu, data[1].second);
    lambda(groupChatMenu, data[2].second);
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
