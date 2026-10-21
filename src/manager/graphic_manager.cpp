#include "graphic_manager.hpp"

#include "application.hpp"
#include "companion.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"
#include "utils.hpp"

GraphicManager::GraphicManager() : manager_(nullptr), mainWindow_(nullptr), map_()
{
    mainWindow_ = std::make_shared<MainWindow>();
}

MainWindowPtr GraphicManager::getMainWindow()
{
    return mainWindow_;
}

void GraphicManager::set()
{
    mainWindow_->set();
    mainWindow_->show();
}

void GraphicManager::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

std::vector<CompanionPtr> GraphicManager::getSortedCompanions()
{
    std::vector<CompanionPtr> data;

    for (const auto &item : map_)
        data.emplace_back(item.second.first);

    auto lambda = [&](auto &item1, auto &item2)
    {
        return item1->id() < item2->id();
    };

    std::ranges::sort(data, lambda);

    return data;
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
    mainWindow_->hideCompanionStub();

    for (const auto &companion : getSortedCompanions()) {
        updateMapping(companion);
        manager_->fillCompanionMessageMapping(companion, false);
    }
}

bool GraphicManager::addCompanion(CompanionPtr companion)
{
    WidgetGroupPtr group = nullptr;
    auto value = std::pair(companion, group);
    auto id = companion->id();
    auto result = map_.emplace(companion->id(), value);

    if (!result.second)
        logTemplateError("companion with id '{}' exists im graphic manager mapping", id);

    return result.second;
}

CompanionPtr GraphicManager::getCompanion(int id) const
{
    auto lambda = [&](int id) { return map_.at(id).first; };

    auto result = runAndReturnSharedPtr<Companion>(lambda, id);

    if (!result)
        logTemplateError("companion with id '{}' not found in manager's map", id);

    return result;
}

CompanionPtr GraphicManager::getCompanionByWidget(SocketInfoBaseWidget *widget) const
{
    auto lambda = [&](const auto &pair)
    {
        return pair.second.second->getSocketInfoBase().get() == widget;
    };

    auto result = std::ranges::find_if(map_, lambda);

    return (result == map_.end()) ? nullptr : result->second.first;
}

WidgetGroupPtr GraphicManager::getWidgetGroup(CompanionPtr companion) const
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

void GraphicManager::updateMapping(CompanionPtr companion)
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

    auto group = getWidgetGroup(selected);
    hideWidgetGroupCentralPanel(group);
}

void GraphicManager::showSelectedCompanionCentralPanel()
{
    auto selected = manager_->getSelectedCompanion();

    if (!selected)
        return;

    auto group = getWidgetGroup(selected);
    showWidgetGroupCentralPanel(group);
}

void GraphicManager::preSelectCompanion(CompanionPtr companion)  // TODO rewrite
{
    if (companion) {
        auto group = getWidgetGroup(companion);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(group->getSocketInfoBase());

        if (cast)
            cast->unselect();

        group->hideCentralPanel();
    }
    else {
        mainWindow_->hideCentralStub();
    }
}

void GraphicManager::postSelectCompanion(CompanionPtr companion)  // TODO rewrite
{
    if (companion) {
        auto group = getWidgetGroup(companion);
        auto cast = dynamic_pointer_cast<SocketInfoWidget>(group->getSocketInfoBase());

        if (cast)
            cast->select();

        group->showCentralPanel();
    }
    else {
        mainWindow_->showCentralStub();
    }
}

void GraphicManager::sendMessage(MessageType type, CompanionPtr companion, const std::string &text)
{
    manager_->sendMessage(type, companion, nullptr, text);
}

void GraphicManager::addLog(const QString &text)
{
    mainWindow_->addLog(text);
}

void GraphicManager::hideWidgetGroupCentralPanel(WidgetGroupPtr group)
{
    group->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(WidgetGroupPtr group)
{
    group->showCentralPanel();
}

void GraphicManager::addWidget(
    MWContainerPosition position, QWidgetPtr widget)
{
    if (!mainWindow_->addWidget(position, widget))
        logArgsError("addWidgetToContainerAndSetParentTo error");
}

void GraphicManager::addCompanionWidget(SocketInfoBaseWidgetPtr widget)
{
    mainWindow_->addCompanion(widget);
}

void GraphicManager::removeCompanionWidget(SocketInfoBaseWidgetPtr widget)
{
    mainWindow_->removeCompanion(widget);
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

void GraphicManager::clearHistory(CompanionPtr companion)
{
    auto group = getWidgetGroup(companion);

    if (group)
        group->clearHistory();
}

void GraphicManager::deleteCompanion(CompanionPtr companion)
{
    manager_->initCompanionDeletion(companion);
}

void GraphicManager::hideInfoViaStubs()
{
    mainWindow_->showStubs();
    hideSelectedCompanionCentralPanel();
}

void GraphicManager::showInfoViaStubs()
{
    mainWindow_->hideStubs();
    showSelectedCompanionCentralPanel();
}

void GraphicManager::hideInfo()
{
    enableMainWindowBlurEffect();
    // hideInfoViaStubs();
}

void GraphicManager::showInfo()
{
    manager_->startUserAuthentication();

    // showInfoViaBlur();
    // showInfoViaStubs();
}

void GraphicManager::enableMainWindowBlurEffect()
{
    mainWindow_->enableBlur();
}

void GraphicManager::disableBlur()
{
    mainWindow_->disableBlur();
}

void GraphicManager::createEntrancePassword()
{
    manager_->initEntrancePasswordCreation();
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
    auto group = getWidgetGroup(companion);

    if (!group)
        return;

    emit group->addMessageWidgetSignal(message);
}

bool GraphicManager::setMessageWidgetAsSent(MessageInfoPtr info)
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

void GraphicManager::setMessageWidgetAsReceived(MessageInfoPtr info)
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

void GraphicManager::updateCompanionWidget(CompanionPtr companion)
{
    auto group = getWidgetGroup(companion);

    if (group)
        group->updateSocketInfoWidget();
}

void GraphicManager::sortHistory(CompanionPtr companion)
{
    auto group = getWidgetGroup(companion);

    if (group)
        group->sortHistory();
}

void GraphicManager::requestHistory(CompanionPtr companion)
{
    manager_->requestHistoryFromCompanion(companion);
}

// bool GraphicManager::connectClient(CompanionPtr companion)
// {
//     return manager_->connectClient(companion);
// }

// bool GraphicManager::disconnectClient(CompanionPtr companion)
// {
//     return manager_->disconnectClient(companion);
// }

bool GraphicManager::clientAction(CompanionPtr companion, bool isConnected)
{
    return manager_->clientAction(companion, isConnected);
}

CompanionDataPtr GraphicManager::getCompanionData(CompanionPtr companion)
{
    return manager_->getCompanionData(companion);
}

void GraphicManager::resetSelectedCompanion(CompanionPtr companion)
{
    manager_->resetSelectedCompanion(companion);
}

void GraphicManager::resetSelectedCompanion(SocketInfoBaseWidget *previous)
{
    auto previousCompanion = getCompanionByWidget(previous);
    resetSelectedCompanion(previousCompanion);
}

bool GraphicManager::setMappedMessageWidget(
    CompanionPtr companion, MessagePtr message, MessageWidgetPtr widget)
{
    return manager_->setMappedMessageWidget(companion, message, widget);
}

void GraphicManager::addEarlyMessages(CompanionPtr companion)
{
    manager_->addEarlyMessages(companion);
}

std::filesystem::path GraphicManager::getLastOpenedPath()
{
    return manager_->getLastOpenedPath();
}

GraphicManagerPtr getGraphicManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp*>(coreApp);

    if (!app)
        return nullptr;

    return app->graphicManager_;
}
