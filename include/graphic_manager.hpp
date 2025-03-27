#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <QString>
#include <QWidget>
#include <string>
#include <vector>

#include "constants.hpp"
#include "mainwindow.hpp"
#include "widgets.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class MainWindow;
class Message;
class MessageWidget;
class PasswordAction;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class WidgetGroup;

class GraphicManager
{
public:
    GraphicManager();
    ~GraphicManager() = default;

    MainWindow* getMainWindowPtr();

    void set();
    void setParentsForStubs(QWidget*, QWidget*);
    void setStubWidgets();
    void sendMessage(MessageType, Companion*, const std::string&);
    void addTextToAppLogWidget(const QString&);
    size_t getCompanionPanelChildrenSize();
    void hideWidgetGroupCentralPanel(WidgetGroup*);
    void showWidgetGroupCentralPanel(WidgetGroup*);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition, QWidget*);

    void addWidgetToCompanionPanel(SocketInfoBaseWidget*);
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidget*);

    void createTextDialogAndShow(
        QWidget*, DialogType, const std::string&, std::vector<ButtonInfo>*);

    void createCompanion();
    void updateCompanion(Companion*);
    void clearCompanionHistory(Companion*);
    void clearChatHistory(WidgetGroup*);
    void deleteCompanion(Companion*);
    void sendCompanionDataToManager(CompanionAction*);
    void showCompanionInfoDialog(CompanionAction*, std::string&&);
    void sendNewPasswordDataToManager(PasswordAction*);
    void sendExistingPasswordDataToManager(PasswordAction*);
    void hideCompanionPanelStub();
    void hideCentralPanelStub();
    void showCentralPanelStub();
    void hideInfoViaBlur();
    void showInfoViaBlur();
    void hideInfoViaStubs();
    void showInfoViaStubs();
    void hideInfo();
    void showInfo();
    void createEntrancePassword();
    void enableMainWindowBlurEffect();
    void disableMainWindowBlurEffect();
    void getEntrancePassword();
    void markMessageWidgetAsSent(Companion*, const Message*);
    void markMessageWidgetAsReceived(Companion*, const Message*);
    void sortChatHistoryElementsForWidgetGroup(WidgetGroup*);
    void sendFile(Companion*);
    void saveFile(const std::string&, Companion*);

private:
    std::mutex messageToMessageWidgetMapMutex_;
    StubWidgetGroup* stubWidgetsPtr_;
    MainWindow* mainWindowPtr_;
};

GraphicManager* getGraphicManagerPtr();

#endif // GRAPHIC_MANAGER_HPP
