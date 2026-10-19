#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"
#include "widgets_panel.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() = default;

    void set();

    void addLog(const QString &text);
    bool addWidget(MWContainerPosition position, QWidgetPtr widget);
    void addCompanion(SocketInfoBaseWidgetPtr widget);
    void removeCompanion(SocketInfoBaseWidgetPtr widget);
    std::size_t getCompanionPanelChildrenSize();
    void enable();
    void disable();
    void enableBlur();
    void disableBlur();
    void hideCentralStub();
    void showCentralStub();
    void hideCompanionStub();
    void showStubs();
    void hideStubs();

private slots:
    void createCompanion();
    void createGroupChat();

private:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void createMenu();

    std::unique_ptr<QPalette> menuBarPalette_;
    std::unique_ptr<MainWidget> mainWidget_;
};

#endif // MAINWINDOW_HPP
