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

    LeftPanelWidget* leftPanelPtr_;

    // QWidget* leftPanelPtr_;
    // QVBoxLayout* leftPanelLayoutPtr_;
    // QPalette* leftPanelPalettePtr_;

    // QWidget* companionPanelPtr_;
    // QVBoxLayout* companionPanelLayoutPtr_;

    // QSpacerItem* spacerPtr_;
    // ShowHideWidget* showHideWidgetPtr_;

    // central panel

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

    RightPanelWidget* rightPanelPtr_;

    // QWidget* rightPanelPtr_;
    // QVBoxLayout* rightPanelLayoutPtr_;
    // QPlainTextEdit* appLogWidgetPtr_;
    // QPalette* appLogWidgetPalettePtr_;

//    QPushButton* testPlainTextEditButton_;

//    void testMainWindowRightPanel();

    void closeEvent(QCloseEvent*) override;

    void keyPressEvent(QKeyEvent*) override;

    void createMenu();

    ~MainWindow();

public:
    MainWindow();

    void addStubWidgetToCompanionPanel();

    void setCompanionPanel();
    void set();

    // void addTextToChatHistoryWidget(const QString&);
    void addTextToAppLogWidget(const QString&);

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    // void addWidgetToCentralPanel(QWidget*);
    void setCentralPanel(CentralPanelWidget*);
    void setRightPanel(RightPanelWidget*);

    size_t getCompanionPanelChildrenSize();

    void removeStubsFromCompanionPanel();
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void oldSelectedCompanionActions(const Companion*);
    void newSelectedCompanionActions(const Companion*);

private slots:
    void createCompanion();
};

// MainWindow* getMainWindowPtr();

//void addTextFunction(const QString&);

#endif // MAINWINDOW_HPP
