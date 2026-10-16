#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

#include "alias.hpp"

class GraphicManager
{
public:
    GraphicManager();
    ~GraphicManager() = default;

    MainWindowPtr getMainWindow();
    void set();
    void setManager(ManagerPtr manager);

    void buildWidgetGroups();
    bool addCompanion(CompanionPtr companion);
    CompanionPtr getMappedCompanionById(int id) const;
    CompanionPtr getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget *widget) const;
    WidgetGroupPtr getMappedWidgetGroupByCompanion(CompanionPtr companion) const;
    CompanionPtr getMappedCompanionByWidgetGroup(WidgetGroupPtr group) const;

    void createWidgetGroupAndAddToMapping(CompanionPtr companion);
    bool deleteWidgetGroup(CompanionPtr companion);

    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();

    void preSelectCompanion(CompanionPtr companion);
    void postSelectCompanion(CompanionPtr companion);

    // void setParentsForStubs(QWidgetPtr leftContainer, QWidgetPtr centralContainer);
    // void setStubWidgets();
    void sendMessage(MessageType type, CompanionPtr companion, const std::string &text);
    void addTextToAppLogWidget(const QString &text);
    void hideWidgetGroupCentralPanel(WidgetGroupPtr group);
    void showWidgetGroupCentralPanel(WidgetGroupPtr group);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition position, QWidgetPtr widget);

    void addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget);
    void removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget);
    void createCompanion();
    void createGroupChat();
    void updateCompanion(CompanionPtr companion);
    void clearCompanionHistory(CompanionPtr companion);
    void clearChatHistory(WidgetGroupPtr widgetGroup);
    void deleteCompanion(CompanionPtr companion);
    // void hideCompanionPanelStub();
    // void hideCentralPanelStub();
    // void showCentralPanelStub();
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
    MessageWidgetPtr buildMessageWidget(CompanionPtr companion, MessagePtr message);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    bool setMessageWidgetAsSent(MessageInfoPtr info);
    void setMessageWidgetAsReceived(MessageInfoPtr info);
    void sortChatHistoryElementsForWidgetGroup(WidgetGroupPtr group);
    void sendFile(CompanionPtr companion);
    void saveFile(const std::string& networkId, CompanionPtr companion);
    std::string getFilePathString(const std::string &networkId);

private:
    // std::mutex messageToMessageWidgetMapMutex_;
    ManagerPtr manager_;
    // std::shared_ptr<StubWidgetGroup> stubWidgets_;
    MainWindowPtr mainWindow_;

    std::unordered_map<int, std::pair<CompanionPtr, WidgetGroupPtr>> map_;
};

GraphicManagerPtr getGraphicManager();

#endif // GRAPHIC_MANAGER_HPP
