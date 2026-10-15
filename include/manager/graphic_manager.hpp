#ifndef GRAPHIC_MANAGER_HPP
#define GRAPHIC_MANAGER_HPP

#include <memory>
#include <string>

#include <QString>
#include <QWidget>

#include "constants.hpp"

class Companion;
class MainWindow;
class Message;
class MessageInfo;
class MessageWidget;
class SocketInfoBaseWidget;
class StubWidgetGroup;
class WidgetGroup;

using CompanionPtr = std::shared_ptr<Companion>;
using MessagePtr = std::shared_ptr<Message>;

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
    void sendMessage(MessageType type, CompanionPtr companion, const std::string &text);
    void addTextToAppLogWidget(const QString &text);
    std::size_t getCompanionPanelChildrenSize();
    void hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group);
    void showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group);

    void addWidgetToMainWindowContainerAndSetParentTo(
        MainWindowContainerPosition position, std::shared_ptr<QWidget> widget);

    void addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);
    void removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget);
    void createCompanion();
    void createGroupChat();
    void updateCompanion(std::shared_ptr<Companion> companion);
    void clearCompanionHistory(std::shared_ptr<Companion> companion);
    void clearChatHistory(std::shared_ptr<WidgetGroup> widgetGroup);
    void deleteCompanion(std::shared_ptr<Companion> companion);
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
    std::shared_ptr<MessageWidget> buildMessageWidget(CompanionPtr companion, MessagePtr message);
    void addMessageWidget(CompanionPtr companion, MessagePtr message);
    bool setMessageWidgetAsSent(std::shared_ptr<MessageInfo> info);
    void setMessageWidgetAsReceived(std::shared_ptr<MessageInfo> info);
    void sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup> group);
    void sendFile(CompanionPtr companion);
    void saveFile(const std::string& networkId, CompanionPtr companion);
    std::string getFilePathString(const std::string &networkId);

private:
    // std::mutex messageToMessageWidgetMapMutex_;
    std::shared_ptr<StubWidgetGroup> stubWidgets_;
    std::shared_ptr<MainWindow> mainWindow_;
};

std::shared_ptr<GraphicManager> getGraphicManager();

#endif // GRAPHIC_MANAGER_HPP
