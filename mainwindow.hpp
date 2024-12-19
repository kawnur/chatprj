#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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

private:
    // TODO rename panel child widgets according its functionality

    QPalette* menuBarPalettePtr_;

    QWidget* centralWidgetPtr_;
    QHBoxLayout* centralWidgetLayoutPtr_;

    // left panel
    // QWidget* leftContainerWidgetPtr_;
    // QVBoxLayout* leftContainerWidgetLayoutPtr_;
    MainWindowContainerWidget* leftContainerWidgetPtr_;

    LeftPanelWidget* leftPanelPtr_;

    // QWidget* leftPanelPtr_;
    // QVBoxLayout* leftPanelLayoutPtr_;
    // QPalette* leftPanelPalettePtr_;

    // QWidget* companionPanelPtr_;
    // QVBoxLayout* companionPanelLayoutPtr_;

    // QSpacerItem* spacerPtr_;
    // ShowHideWidget* showHideWidgetPtr_;

    // central panel
    // QWidget* centralContainerWidgetPtr_;
    // QVBoxLayout* centralContainerWidgetLayoutPtr_;
    MainWindowContainerWidget* centralContainerWidgetPtr_;

    CentralPanelWidget* centralPanelPtr_;

//     QWidget* centralPanelPtr_;
//     QVBoxLayout* centralPanelLayoutPtr_;
// //    QGraphicsScene* graphicsScene_;
// //    QGraphicsView* graphicsView_;
// //    QRectF* rect_;
// //    QGraphicsSimpleTextItem* textItem_;
// //    int linesCount_;
// //    QFont* font_;

//     QLabel* companionNameLabelPtr_;
//     QPalette* companionNameLabelPalettePtr_;
//     QPlainTextEdit* chatHistoryWidgetStubPtr_;
//     QPalette* chatHistoryWidgetStubPalettePtr_;

//     TextEditWidget* textEditStubPtr_;
//     QPalette* textEditStubPalettePtr_;
//    QPushButton* button_;

    // right panel
    // QWidget* rightContainerWidgetPtr_;
    // QVBoxLayout* rightContainerWidgetLayoutPtr_;
    MainWindowContainerWidget* rightContainerWidgetPtr_;

    RightPanelWidget* rightPanelPtr_;

    // QWidget* rightPanelPtr_;
    // QVBoxLayout* rightPanelLayoutPtr_;
    // QPlainTextEdit* appLogWidgetPtr_;
    // QPalette* appLogWidgetPalettePtr_;

//    QPushButton* testPlainTextEditButton_;

//    void testMainWindowRightPanel();

    ShowHideWidget* showHideWidgetPtr_;

    void closeEvent(QCloseEvent*) override;

    void keyPressEvent(QKeyEvent*) override;

    void createMenu();

    ~MainWindow();

public:
    MainWindow();

    // void addStubWidgetToCompanionPanel();

    // void setCompanionPanel();
    void set();

    void addLeftPanelToLayout();

    // void addTextToChatHistoryWidget(const QString&);
    void addTextToAppLogWidget(const QString&);

    void addWidgetToLeftContainerAndSetParentTo(QWidget*);
    void addWidgetToCentralContainerAndSetParentTo(QWidget*);
    void addWidgetToRightContainerAndSetParentTo(QWidget*);

    // void addWidgetToCentralWidgetLayout(QWidget*);
    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    // void addWidgetToCentralPanel(QWidget*);
    void setLeftPanel(LeftPanelWidget*);
    void setCentralPanel(CentralPanelWidget*);
    void setRightPanel(RightPanelWidget*);

    size_t getCompanionPanelChildrenSize();

    void removeStubsFromCompanionPanel();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void hideLeftAndRightPanels();
    void showLeftAndRightPanels();

    // void oldSelectedCompanionActions(const Companion*);
    // void newSelectedCompanionActions(const Companion*);

private slots:
    void createCompanion();
};

// MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
