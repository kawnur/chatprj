#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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
#include <QVBoxLayout>

#include "application.hpp"
#include "db_interaction.hpp"
#include "manager.hpp"
#include "utils_cout.hpp"
#include "widgets.hpp"

class LeftPanelWidget;
class MainWindowContainerWidget;
class ShowHideWidget;
class SocketInfoBaseWidget;
class SocketInfoWidget;
class TextEditWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void set();

    void addTextToAppLogWidget(const QString&);

    void addWidgetToContainerAndSetParentTo(MainWindowContainerPosition, QWidget*);

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);

    size_t getCompanionPanelChildrenSize();

    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void hideLeftAndRightPanels();
    void showLeftAndRightPanels();

    int getLeftPanelWidgetWidth();

    void setGraphicsEffectToNullptr();

private slots:
    void createCompanion();

private:
    // TODO rename panel child widgets according its functionality

    QPalette* menuBarPalettePtr_;

    QWidget* centralWidgetPtr_;
    QHBoxLayout* centralWidgetLayoutPtr_;

    // left panel
    MainWindowContainerWidget* leftContainerWidgetPtr_;
    LeftPanelWidget* leftPanelPtr_;

    // central panel
    MainWindowContainerWidget* centralContainerWidgetPtr_;
    CentralPanelWidget* centralPanelPtr_;

    // right panel
    MainWindowContainerWidget* rightContainerWidgetPtr_;
    RightPanelWidget* rightPanelPtr_;

    ShowHideWidget* showHideWidgetPtr_;

    std::map<MainWindowContainerPosition, MainWindowContainerWidget*>
        mapContainerPtrToContainerPosition;

    QGraphicsBlurEffect* blurEffectPtr_;

    void closeEvent(QCloseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

    void createMenu();
};

#endif // MAINWINDOW_HPP
