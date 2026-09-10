#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <map>
#include <memory>

#include <QGraphicsBlurEffect>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QWidget>

#include "constants.hpp"

class CentralPanelWidget;
class LeftPanelWidget;
class MainWindowContainerWidget;
class RightPanelWidget;
class ShowHideWidget;
class SocketInfoBaseWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() = default;

    void set();
    void addTextToAppLogWidget(const QString &text);

    void addWidgetToContainerAndSetParentTo(
        MainWindowContainerPosition position, std::shared_ptr<QWidget> widget);

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);
    std::size_t getCompanionPanelChildrenSize();
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);
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
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void createMenu();
    void setBlurEffect();

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

    // containerMap
    std::map<MainWindowContainerPosition, std::shared_ptr<MainWindowContainerWidget>> containerMap;

    std::shared_ptr<QGraphicsBlurEffect> blurEffect_;
    std::unique_ptr<QPalette> menuBarPalette_;
    std::shared_ptr<QWidget> centralWidget_;
    std::unique_ptr<QHBoxLayout> centralWidgetLayout_;
    std::unique_ptr<QSplitter> splitter_;
};

#endif // MAINWINDOW_HPP
