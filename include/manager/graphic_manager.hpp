#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <string>
#include <unordered_map>

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

using GraphicManagerMapValue = std::pair<CompanionPtr, WidgetGroupPtr>;
using GraphicManagerMap = std::unordered_map<int, GraphicManagerMapValue>;

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
    CompanionPtr getCompanion(int id) const;
    void updateMapping(CompanionPtr companion);
    void deleteWidgetGroup(CompanionPtr companion);
    void preSelectCompanion();
    void postSelectCompanion();
    void sendMessage(MessageType type, CompanionPtr companion, const std::string &text);
    void addLog(const QString &text);
    void addWidget(MWContainerPosition position, QWidgetPtr widget);
    void addCompanionWidget(SocketInfoBaseWidgetPtr widget);
    void removeCompanionWidget(SocketInfoBaseWidgetPtr widget);
    void createCompanion();
    void createGroupChat();
    void updateCompanion(CompanionPtr companion);
    void clearCompanionHistory(CompanionPtr companion);
    void clearHistory(CompanionPtr companion);
    void deleteCompanion(CompanionPtr companion);
    void hideInfo();
    void showInfo();
    void enableMainWindowBlurEffect();
    void disableBlur();
    // void getEntrancePassword();
    MessageWidgetPtr buildMessageWidget(CompanionPtr companion, MessagePtr message);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    void addMessageWidgets(CompanionPtr companion, const std::vector<MessagePtr> &messages);
    void setMessageWidgetState(MessageWidgetState state, MessageInfoPtr info);
    void sendFile(CompanionPtr companion);
    void saveFile(const std::string& networkId, CompanionPtr companion);
    std::string getFilePathString(const std::string &networkId);
    void updateCompanionWidget(CompanionPtr companion);
    void requestHistory(CompanionPtr companion);
    bool clientAction(CompanionPtr companion, bool isConnected);
    CompanionDataPtr getCompanionData(CompanionPtr companion);
    void resetSelectedCompanion(CompanionPtr companion);
    void resetSelectedCompanion(SocketInfoBaseWidget *widget);

    bool setMappedMessageWidget(
        CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget);

    void addEarlyMessages(CompanionPtr companion);
    std::filesystem::path getLastOpenedPath();

private:
    std::optional<GraphicManagerMapValue> getMappedValue(int id) const;
    WidgetGroupPtr getWidgetGroup(CompanionPtr companion) const;
    std::vector<CompanionPtr> getSortedCompanions();
    void hideSelectedCompanionCentralPanel();
    void showSelectedCompanionCentralPanel();
    void hideWidgetGroupCentralPanel(WidgetGroupPtr group);
    void showWidgetGroupCentralPanel(WidgetGroupPtr group);
    void hideInfoViaStubs();
    void showInfoViaStubs();
    void createEntrancePassword();

    // std::mutex messageToMessageWidgetMapMutex_;
    ManagerPtr manager_;
    MainWindowPtr mainWindow_;
    GraphicManagerMap map_;
    CompanionPtr selectedCompanion_;
};

GraphicManagerPtr getGraphicManager();

#endif // GRAPHIC_MANAGER_HPP
