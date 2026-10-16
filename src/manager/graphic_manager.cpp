#include "graphic_manager.hpp"

#include "application.hpp"
#include "companion.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"
#include "utils.hpp"

GraphicManager::GraphicManager() : map_()
{
    // stubWidgets_ = std::make_shared<StubWidgetGroup>();
    mainWindow_ = std::make_shared<MainWindow>();
}

MainWindowPtr GraphicManager::getMainWindow()
{
    return mainWindow_;
}

void GraphicManager::set()
{
    // stubWidgets_->set();
    mainWindow_->set();
    mainWindow_->show();
}

void GraphicManager::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

void GraphicManager::buildWidgetGroups()
{
    auto companionsNumber = map_.size();
    auto childrenSize = mainWindow_->getCompanionPanelChildrenSize();

    logTemplateInfo("companionsNumber: {0}, childrenSize: {1}", companionsNumber, childrenSize);

    if (companionsNumber == 0 && childrenSize == 0) {
        logArgsWarning("strange case, empty sockets panel");

        return;
    }

    // TODO check if sockets already are children

    // hide companion panel stub widget
    // hideCompanionPanelStub();
    mainWindow_->hideCompanionPanelStub();

    for (const auto &pair : map_) {
        auto companion = pair.second.first;
        createWidgetGroupAndAddToMapping(companion);
        manager_->fillCompanionMessageMapping(companion, false);
    }
}

bool GraphicManager::addCompanion(CompanionPtr companion)
{
    WidgetGroupPtr group = nullptr;
    auto value = std::pair(companion, group);
    auto result = map_.emplace(companion->id(), value);

    if (!result.second)
        logArgsError("companion is nullptr");

    return result.second;
}

CompanionPtr GraphicManager::getMappedCompanionById(int id) const
{
    auto lambda = [&](int id) { return map_.at(id).first; };

    auto result = runAndReturnSharedPtr<Companion>(lambda, id);

    if (!result)
        logTemplateError("companion with id '{}' not found in manager's map", id);

    return result;
}

CompanionPtr GraphicManager::getMappedCompanionBySocketInfoBaseWidget(SocketInfoBaseWidget *widget) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second->getSocketInfoBase().get() == widget;
    };

    auto result = std::ranges::find_if(map_, lambda);

    return (result == map_.end()) ? nullptr : result->second.first;
}

WidgetGroupPtr GraphicManager::getMappedWidgetGroupByCompanion(CompanionPtr companion) const
{
    WidgetGroupPtr group = nullptr;

    auto id = companion->id();

    try {
        group = map_.at(id).second;
    } catch(std::out_of_range) {
        logTemplateError("mapCompanionToWidgetGroup_ does not conain id {0}", id);

        return nullptr;
    }

    return group;
}

CompanionPtr GraphicManager::getMappedCompanionByWidgetGroup(WidgetGroupPtr group) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second == group;
    };

    auto result = std::ranges::find_if(map_, lambda);

    return result->second.first;
}

void GraphicManager::createWidgetGroupAndAddToMapping(CompanionPtr companion)
{
    auto group = std::make_shared<WidgetGroup>(companion);
    group->set();

    // no existance check
    map_[companion->id()].second = group;

    // companion->addMessageWidgetsToChatHistory(group);  // ???
}

bool GraphicManager::deleteWidgetGroup(CompanionPtr companion)
{
    auto lambda = [&](const auto &iterator)
    {
        return iterator.second.first == companion;
    };

    // TODO use range
    auto result = std::ranges::find_if(map_, lambda);

    if (result == map_.end()) {
        // showErrorDialogAndLogError("Companion was not found in mapping at deletion");
        return false;
    }
    else {
        // if (selectedCompanion_ == companion)
        //     selectedCompanion_ = nullptr;

        map_.erase(result);

        return true;
    }
}

void GraphicManager::hideSelectedCompanionCentralPanel()
{
    auto selected = manager_->getSelectedCompanion();

    if (!selected)
        return;

    auto group = getMappedWidgetGroupByCompanion(selected);
    hideWidgetGroupCentralPanel(group);
}

void GraphicManager::showSelectedCompanionCentralPanel()
{
    auto selected = manager_->getSelectedCompanion();

    if (!selected)
        return;

    auto group = getMappedWidgetGroupByCompanion(selected);
    showWidgetGroupCentralPanel(group);
}

void GraphicManager::preSelectCompanion(CompanionPtr companion)  // TODO rewrite
{
    if (companion) {
        auto group = getMappedWidgetGroupByCompanion(companion);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(group->getSocketInfoBase());

        if (cast)
            cast->unselect();

        group->hideCentralPanel();
    }
    else {
        // hideCentralPanelStub();
        mainWindow_->hideCentralPanelStub();
    }
}

void GraphicManager::postSelectCompanion(CompanionPtr companion)  // TODO rewrite
{
    if (companion) {
        auto group = getMappedWidgetGroupByCompanion(companion);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(group->getSocketInfoBase());

        if (cast)
            cast->select();

        group->showCentralPanel();
    }
    else {
        // showCentralPanelStub();
        mainWindow_->showCentralPanelStub();
    }
}

// void GraphicManager::setParentsForStubs(QWidgetPtr leftContainer, QWidgetPtr centralContainer)
// {
//     stubWidgets_->setParents(leftContainer, centralContainer);
// }

// void GraphicManager::setStubWidgets()
// {
//     stubWidgets_->set();
// }

void GraphicManager::sendMessage(MessageType type, CompanionPtr companion, const std::string &text)
{
    manager_->sendMessage(type, companion, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString &text)
{
    mainWindow_->addTextToAppLogWidget(text);
}

void GraphicManager::hideWidgetGroupCentralPanel(WidgetGroupPtr group)
{
    group->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(WidgetGroupPtr group)
{
    group->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, QWidgetPtr widget)
{
    if (!mainWindow_->addWidgetToContainerAndSetParentTo(position, widget))
        logArgsError("addWidgetToContainerAndSetParentTo error");
}

void GraphicManager::addWidgetToCompanionPanel(SocketInfoBaseWidgetPtr widget)
{
    mainWindow_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::removeWidgetFromCompanionPanel(SocketInfoBaseWidgetPtr widget)
{
    mainWindow_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::createCompanion()
{
    manager_->initCompanionCreation();
}

void GraphicManager::createGroupChat()
{
    // auto action = std::make_shared<GroupChatAction>(ChatActionType::CREATE);
    // action->set();
}

void GraphicManager::updateCompanion(CompanionPtr companion)
{
    manager_->initCompanionUpdate(companion);
}

void GraphicManager::clearCompanionHistory(CompanionPtr companion)
{
    manager_->initCompanionHistoryClearing(companion);
}

void GraphicManager::clearChatHistory(WidgetGroupPtr widgetGroup)
{
    widgetGroup->clearChatHistory();
}

void GraphicManager::deleteCompanion(CompanionPtr companion)
{
    manager_->initCompanionDeletion(companion);
}

// void GraphicManager::hideCompanionPanelStub()
// {
//     stubWidgets_->hideSocketInfoStubWidget();
// }

// void GraphicManager::hideCentralPanelStub()
// {
//     stubWidgets_->hideCentralPanel();
// }

// void GraphicManager::showCentralPanelStub()
// {
//     stubWidgets_->showCentralPanel();
// }

void GraphicManager::hideInfoViaBlur()
{
    enableMainWindowBlurEffect();
}

void GraphicManager::showInfoViaBlur()
{
    disableMainWindowBlurEffect();
}

void GraphicManager::hideInfoViaStubs()
{
    // stubWidgets_->setLeftPanelWidth(mainWindow_->getLeftPanelWidgetWidth());
    // mainWindow_->hideLeftAndRightPanels();
    // hideSelectedCompanionCentralPanel();
    // stubWidgets_->showStubPanels();
    mainWindow_->showStubPanels();
    hideSelectedCompanionCentralPanel();
}

void GraphicManager::showInfoViaStubs()
{
    // mainWindow_->showLeftAndRightPanels();
    // showSelectedCompanionCentralPanel();
    // stubWidgets_->hideStubPanels();
    mainWindow_->hideStubPanels();
    showSelectedCompanionCentralPanel();
}

void GraphicManager::hideInfo()
{
    hideInfoViaBlur();
    // hideInfoViaStubs();
}

void GraphicManager::showInfo()
{
    manager_->startUserAuthentication();

    // showInfoViaBlur();
    // showInfoViaStubs();
}

void GraphicManager::createEntrancePassword()
{
    manager_->initEntrancePasswordCreation();
}

void GraphicManager::enableMainWindowBlurEffect()
{
    mainWindow_->enableBlurEffect();
}

void GraphicManager::disableMainWindowBlurEffect()
{
    mainWindow_->disableBlurEffect();
}

void GraphicManager::getEntrancePassword()
{
    manager_->initEntrancePasswordReception();
}

MessageWidgetPtr GraphicManager::buildMessageWidget(CompanionPtr companion, MessagePtr message)
{
    switch (message->type()) {
    case MessageType::TEXT:
        return std::make_shared<TextMessageWidget>(companion, message);

    case MessageType::FILE:
        return std::make_shared<FileMessageWidget>(companion, message);

    default:
        logArgsError("message widget building error");

        return nullptr;
    }
}

void GraphicManager::addMessageWidget(CompanionPtr companion, MessagePtr message)
{
    auto group = getMappedWidgetGroupByCompanion(companion);

    if (!group)
        return;

    emit group->addMessageWidgetSignal(message);
    // group->addMessageWidget(message);
}

bool GraphicManager::setMessageWidgetAsSent(
    // CompanionPtr companion, MessagePtr message)
    MessageInfoPtr info)
{
    // auto lambda = [&]()
    // {
    //     // std::lock_guard<std::mutex> lock(messageToMessageWidgetMapMutex_);
    //     companion->getMappedMessageWidgetByMessage(message)->setMessageWidgetAsSent();
    // };

    // return runAndReturnBool(lambda);
    auto widget = info->widget();

    if (widget)
        widget->setSent();
    else
        logArgsError("marking widget as sent error");

    return true;  // TODO modify
}

void GraphicManager::setMessageWidgetAsReceived(
    // CompanionPtr companion, MessagePtr message)
    MessageInfoPtr info)
{
    // auto lambda = [&]()
    // {
    //     // std::lock_guard<std::mutex> lock(messageToMessageWidgetMapMutex_);
    //     companion->getMappedMessageWidgetByMessage(message)->setMessageWidgetAsReceived();
    // };

    // return runAndReturnBool(lambda);
    auto widget = info->widget();

    if (widget)
        widget->setSent();
    else
        logArgsError("marking widget as received error");
}

void GraphicManager::sortChatHistoryElementsForWidgetGroup(WidgetGroupPtr group)
{
    group->sortChatHistoryElements();
}

void GraphicManager::sendFile(CompanionPtr companion)
{
    manager_->initFileSend(companion);
}

void GraphicManager::saveFile(const std::string &networkId, CompanionPtr companion)
{
    manager_->initFileReception(networkId, companion);
}

std::string GraphicManager::getFilePathString(const std::string &networkId)
{
    return manager_->getFilePathString(networkId);
}

GraphicManagerPtr getGraphicManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp*>(coreApp);

    if (!app)
        return nullptr;

    return app->graphicManager_;
}
