#include "graphic_manager.hpp"

#include "application.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "widgets.hpp"
#include "widgets_message.hpp"
#include "utils.hpp"

GraphicManager::GraphicManager()
{
    stubWidgets_ = std::make_shared<StubWidgetGroup>();
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

void GraphicManager::setParentsForStubs(QWidgetPtr leftContainer, QWidgetPtr centralContainer)
{
    stubWidgets_->setParents(leftContainer, centralContainer);
}

void GraphicManager::setStubWidgets()
{
    stubWidgets_->set();
}

void GraphicManager::sendMessage(MessageType type, CompanionPtr companion, const std::string &text)
{
    getManager()->sendMessage(type, companion, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString &text)
{
    mainWindow_->addTextToAppLogWidget(text);
}

std::size_t GraphicManager::getCompanionPanelChildrenSize()
{
    return mainWindow_->getCompanionPanelChildrenSize();
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
    getManager()->initCompanionCreation();
}

void GraphicManager::createGroupChat()
{
    // auto action = std::make_shared<GroupChatAction>(ChatActionType::CREATE);
    // action->set();
}

void GraphicManager::updateCompanion(CompanionPtr companion)
{
    getManager()->initCompanionUpdate(companion);
}

void GraphicManager::clearCompanionHistory(CompanionPtr companion)
{
    getManager()->initCompanionHistoryClearing(companion);
}

void GraphicManager::clearChatHistory(WidgetGroupPtr widgetGroup)
{
    widgetGroup->clearChatHistory();
}

void GraphicManager::deleteCompanion(CompanionPtr companion)
{
    getManager()->initCompanionDeletion(companion);
}

void GraphicManager::hideCompanionPanelStub()
{
    stubWidgets_->hideSocketInfoStubWidget();
}

void GraphicManager::hideCentralPanelStub()
{
    stubWidgets_->hideCentralPanel();
}

void GraphicManager::showCentralPanelStub()
{
    stubWidgets_->showCentralPanel();
}

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
    stubWidgets_->setLeftPanelWidth(mainWindow_->getLeftPanelWidgetWidth());
    mainWindow_->hideLeftAndRightPanels();
    getManager()->hideSelectedCompanionCentralPanel();
    stubWidgets_->showStubPanels();
}

void GraphicManager::showInfoViaStubs()
{
    mainWindow_->showLeftAndRightPanels();
    getManager()->showSelectedCompanionCentralPanel();
    stubWidgets_->hideStubPanels();
}

void GraphicManager::hideInfo()
{
    hideInfoViaBlur();
    // hideInfoViaStubs();
}

void GraphicManager::showInfo()
{
    getManager()->startUserAuthentication();

    // showInfoViaBlur();
    // showInfoViaStubs();
}

void GraphicManager::createEntrancePassword()
{
    getManager()->initEntrancePasswordCreation();
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
    getManager()->initEntrancePasswordReception();
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
    auto group = getManager()->getMappedWidgetGroupByCompanion(companion);

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
    getManager()->initFileSend(companion);
}

void GraphicManager::saveFile(const std::string &networkId, CompanionPtr companion)
{
    getManager()->initFileReception(networkId, companion);
}

std::string GraphicManager::getFilePathString(const std::string &networkId)
{
    return getManager()->getFilePathString(networkId);
}

GraphicManagerPtr getGraphicManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp*>(coreApp);

    if (!app)
        return nullptr;

    return app->graphicManager_;
}
