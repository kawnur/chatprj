#include "graphic_manager.hpp"

#include "application.hpp"
#include "companion.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"
#include "utils.hpp"

GraphicManager::GraphicManager()
    : manager_(nullptr), mainWindow_(nullptr), map_(), selectedCompanion_(nullptr)
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

    // TODO check if sockets already are children
    mainWindow_->hideCompanionStub();

    for (const auto &companion : getSortedCompanions()) {
        updateMapping(companion);
        manager_->fillCompanionMessageMapping(companion, true);
    }
}

bool GraphicManager::addCompanion(CompanionPtr companion)
{
    WidgetGroupPtr group = nullptr;
    auto value = std::pair(companion, group);
    auto id = companion->id();
    auto result = map_.emplace(companion->id(), value);

    if (!result.second)
        logTemplateError("companion with id '{}' exists in graphic manager mapping", id);

    return result.second;
}

std::optional<GraphicManagerMapValue> GraphicManager::getMappedValue(int id) const
{
    auto lambda = [&]() { return map_.at(id); };
    auto result = runAndReturnOptional<GraphicManagerMapValue>(lambda);

    if (!result)
        logTemplateError("companion with id '{}' not found", id);

    return result;
}

CompanionPtr GraphicManager::getCompanion(int id) const
{
    auto value = getMappedValue(id);

    return (value) ? value.value().first : nullptr;
}

WidgetGroupPtr GraphicManager::getWidgetGroup(CompanionPtr companion) const
{
    auto value = getMappedValue(companion->id());

    return (value) ? value.value().second : nullptr;
}

void GraphicManager::updateMapping(CompanionPtr companion)
{
    auto group = std::make_shared<WidgetGroup>(companion);

    if (!group)
        return;

    group->set();

    // no existance check
    map_[companion->id()].second = group;

    // companion->addMessageWidgetsToChatHistory(group);  // ???
}

void GraphicManager::deleteWidgetGroup(CompanionPtr companion)
{
    auto lambda = [&](const auto &iterator)
    {
        return iterator.second.first == companion;
    };

    auto result = std::ranges::find_if(map_, lambda);

    if (result == map_.end())
        return;

    map_.erase(result);

    if (selectedCompanion_ == companion)
        selectedCompanion_ = nullptr;
}

void GraphicManager::hideSelectedCompanionCentralPanel()
{
    if (!selectedCompanion_)
        return;

    auto group = getWidgetGroup(selectedCompanion_);

    if (!group)
        return;

    hideWidgetGroupCentralPanel(group);
}

void GraphicManager::showSelectedCompanionCentralPanel()
{
    if (!selectedCompanion_)
        return;

    auto group = getWidgetGroup(selectedCompanion_);

    if (!group)
        return;

    showWidgetGroupCentralPanel(group);
}

void GraphicManager::preSelectCompanion()
{
    if (!selectedCompanion_) {
        mainWindow_->hideCentralStub();

        return;
    }

    auto group = getWidgetGroup(selectedCompanion_);

    if (!group)
        return;

    group->unselect();
    group->hideCentralPanel();
}

void GraphicManager::postSelectCompanion()
{
    if (!selectedCompanion_) {
        mainWindow_->showCentralStub();

        return;
    }

    auto group = getWidgetGroup(selectedCompanion_);

    if (!group)
        return;

    group->select();
    group->showCentralPanel();
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

void GraphicManager::addWidget(MWContainerPosition position, QWidgetPtr widget)
{
    if (!mainWindow_->addWidget(position, widget))
        logArgsError(Q_FUNC_INFO, "error");
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

    if (!group)
        return;

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

// void GraphicManager::getEntrancePassword()
// {
//     manager_->initEntrancePasswordReception();
// }

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

void GraphicManager::addMessageWidgets(
    CompanionPtr companion, const std::vector<MessagePtr> &messages)
{
    auto group = getWidgetGroup(companion);

    if (!group)
        return;

    emit group->addMessageWidgetsSignal(messages);
}

void GraphicManager::setMessageWidgetState(MessageWidgetState state, MessageInfoPtr info)
{
    auto widget = info->widget();

    if (!widget) {
        logArgsError("message widget state setting error");

        return;
    }

    switch (state) {
    case MessageWidgetState::SENT:
        widget->setSent();

    break;

    case MessageWidgetState::RECEIVED:
        widget->setReceived();

    break;

    default:
        break;
    }
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

    if (!group)
        return;

    group->updateSocketInfoWidget();
}

void GraphicManager::requestHistory(CompanionPtr companion)
{
    manager_->requestHistoryFromCompanion(companion);
}

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
    preSelectCompanion();
    selectedCompanion_ = companion;
    postSelectCompanion();
}

void GraphicManager::resetSelectedCompanion(SocketInfoBaseWidget *widget)
{
    auto companion = (widget) ? widget->companion() : nullptr;
    resetSelectedCompanion(companion);
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
