#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include <QString>
#include <QWidget>

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

class GraphicManager {
public:
    GraphicManager();
    ~GraphicManager() = default;

    std::shared_ptr<MainWindow> getMainWindow();

    void set();
    void setParentsForStubs(std::shared_ptr<QWidget>, std::shared_ptr<QWidget>);
    void setStubWidgets();
    void sendMessage(MessageType, std::shared_ptr<Companion>, const std::string&);
    void addTextToAppLogWidget(const QString&);
    std::size_t getCompanionPanelChildrenSize();
    void hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup>);
    void showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup>);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition, std::shared_ptr<QWidget>);

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget>);

    void createTextDialogAndShow(
        std::shared_ptr<QWidget>, DialogType, const std::string&, std::vector<ButtonInfo>*);

    void createCompanion();
    void createGroupChat();
    void updateCompanion(std::shared_ptr<Companion>);
    void clearCompanionHistory(std::shared_ptr<Companion>);
    void clearChatHistory(std::shared_ptr<WidgetGroup>);
    void deleteCompanion(std::shared_ptr<Companion>);
    void sendCompanionDataToManager(std::shared_ptr<CompanionAction>);
    void showCompanionInfoDialog(std::shared_ptr<CompanionAction>, std::string&&);
    void sendNewPasswordDataToManager(std::shared_ptr<PasswordAction>);
    void sendExistingPasswordDataToManager(std::shared_ptr<PasswordAction>);
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
    void markMessageWidgetAsSent(std::shared_ptr<Companion>, std::shared_ptr<Message>);
    void markMessageWidgetAsReceived(std::shared_ptr<Companion>, std::shared_ptr<Message>);
    void sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup>);
    void sendFile(std::shared_ptr<Companion>);
    void saveFile(const std::string&, std::shared_ptr<Companion>);

private:
    std::mutex messageToMessageWidgetMapMutex_;
    std::shared_ptr<StubWidgetGroup> stubWidgets_;
    std::shared_ptr<MainWindow> mainWindow_;
};

std::shared_ptr<GraphicManager> getGraphicManager();

#endif // GRAPHIC_MANAGER_HPP
