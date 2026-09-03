#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSpacerItem>
#include <QSplitter>
#include <QVBoxLayout>

#include "application.hpp"
#include "db_interaction.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"
#include "widgets.hpp"

class CentralPanelWidget;
class LeftPanelWidget;
class MainWindowContainerWidget;
class RightPanelWidget;
class ShowHideWidget;
class SocketInfoBaseWidget;
class SocketInfoWidget;
class TextEditWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() = default;

    void set();
    void addTextToAppLogWidget(const QString&);
    void addWidgetToContainerAndSetParentTo(MainWindowContainerPosition, std::shared_ptr<QWidget>);
    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    void hideLeftAndRightPanels();
    void showLeftAndRightPanels();
    int getLeftPanelWidgetWidth();
    void enableWidgetsForShowHide();
    void disableWidgetsForShowHide();
    void enableBlurEffect();
    void disableBlurEffect();

private slots:
    void createCompanion();
    void createGroupChat();

private:
    std::shared_ptr<QPalette> menuBarPalette_;
    std::shared_ptr<QWidget> centralWidget_;
    std::shared_ptr<QHBoxLayout> centralWidgetLayout_;
    std::shared_ptr<QSplitter> splitter_;

    // left panel
    std::shared_ptr<MainWindowContainerWidget> leftContainerWidget_;
    std::shared_ptr<LeftPanelWidget> leftPanel_;

    // central panel
    std::shared_ptr<MainWindowContainerWidget> centralContainerWidget_;
    std::shared_ptr<CentralPanelWidget> centralPanel_;

    // right panel
    std::shared_ptr<MainWindowContainerWidget> rightContainerWidget_;
    std::shared_ptr<RightPanelWidget> rightPanel_;

    std::shared_ptr<ShowHideWidget> showHideWidget_;

    std::map<MainWindowContainerPosition, std::shared_ptr<MainWindowContainerWidget>>
        mapContainerToContainerPosition;

    std::shared_ptr<QGraphicsBlurEffect> blurEffect_;

    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void createMenu();
    void setBlurEffect();
};

#endif // MAINWINDOW_HPP
