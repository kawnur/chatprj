#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <QString>
#include <QWidget>

#include "constants.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class MainWindow;
class Message;
class PasswordAction;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class WidgetGroup;

class GraphicManager
{
public:
    GraphicManager();
    ~GraphicManager() = default;

    std::shared_ptr<MainWindow> getMainWindow();

    void set();

    void setParentsForStubs(
        std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer);

    void setStubWidgets();

    void sendMessage(
        MessageType type, std::shared_ptr<Companion> companion, const std::string &text);

    void addTextToAppLogWidget(const QString &text);
    std::size_t getCompanionPanelChildrenSize();
    void hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group);
    void showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition position, std::shared_ptr<QWidget> widget);

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);

    void createTextDialogAndShow(
        std::shared_ptr<QWidget> parent, DialogType type, const std::string &text,
        std::shared_ptr<std::vector<ButtonInfo>> buttonInfo);

    void createCompanion();
    void createGroupChat();
    void updateCompanion(std::shared_ptr<Companion> companion);
    void clearCompanionHistory(std::shared_ptr<Companion> companion);
    void clearChatHistory(std::shared_ptr<WidgetGroup> widgetGroup);
    void deleteCompanion(std::shared_ptr<Companion> companion);
    void sendCompanionDataToManager(std::shared_ptr<CompanionAction> action);
    void showCompanionInfoDialog(std::shared_ptr<CompanionAction> action, std::string &&header);
    void sendNewPasswordDataToManager(std::shared_ptr<PasswordAction> action);
    void sendExistingPasswordDataToManager(std::shared_ptr<PasswordAction> action);
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

    bool markMessageWidgetAsSent(
        std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

    bool markMessageWidgetAsReceived(
        std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

    void sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup> group);
    void sendFile(std::shared_ptr<Companion> companion);
    void saveFile(const std::string& networkId, std::shared_ptr<Companion> companion);

private:
    // std::mutex messageToMessageWidgetMapMutex_;
    std::shared_ptr<StubWidgetGroup> stubWidgets_;
    std::shared_ptr<MainWindow> mainWindow_;
};

std::shared_ptr<GraphicManager> getGraphicManager();

#endif // GRAPHIC_MANAGER_HPP
