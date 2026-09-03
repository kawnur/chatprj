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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

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
    std::shared_ptr<QPalette> menuBarPalettePtr_;
    std::shared_ptr<QWidget> centralWidgetPtr_;
    std::shared_ptr<QHBoxLayout> centralWidgetLayoutPtr_;
    std::shared_ptr<QSplitter> splitterPtr_;

    // left panel
    std::shared_ptr<MainWindowContainerWidget> leftContainerWidgetPtr_;
    std::shared_ptr<LeftPanelWidget> leftPanelPtr_;

    // central panel
    std::shared_ptr<MainWindowContainerWidget> centralContainerWidgetPtr_;
    std::shared_ptr<CentralPanelWidget> centralPanelPtr_;

    // right panel
    std::shared_ptr<MainWindowContainerWidget> rightContainerWidgetPtr_;
    std::shared_ptr<RightPanelWidget> rightPanelPtr_;

    std::shared_ptr<ShowHideWidget> showHideWidgetPtr_;

    std::map<MainWindowContainerPosition, std::shared_ptr<MainWindowContainerWidget>>
        mapContainerPtrToContainerPosition;

    std::shared_ptr<QGraphicsBlurEffect> blurEffectPtr_;

    void closeEvent(std::shared_ptr<QCloseEvent>) override;
    void keyPressEvent(std::shared_ptr<QKeyEvent>) override;
    void mouseDoubleClickEvent(std::shared_ptr<QMouseEvent>) override;
    void createMenu();
    void setBlurEffect();
};

#endif // MAINWINDOW_HPP
