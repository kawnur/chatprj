#include "graphic_manager.hpp"

GraphicManager::GraphicManager() : messageToMessageWidgetMapMutex_(std::mutex())
{
    // mapMessageToMessageWidget_(std::map<std::shared_ptr<Message>, std::shared_ptr<MessageWidget>>()) {
    // stubWidgets_ = new StubWidgetGroup;
    // mainWindow_ = new MainWindow;

    // stubWidgets_ = nullptr;
    // mainWindow_ = nullptr;
}

std::shared_ptr<MainWindow> GraphicManager::getMainWindow()
{
    return mainWindow_;
}

void GraphicManager::set()
{
    stubWidgets_ = std::make_shared<StubWidgetGroup>();
    mainWindow_ = std::make_shared<MainWindow>();

    // stubWidgets_->set();
    mainWindow_->set();
    mainWindow_->show();
}

void GraphicManager::setParentsForStubs(
    std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer)
{
    stubWidgets_->setParents(leftContainer, centralContainer);
}

void GraphicManager::setStubWidgets()
{
    stubWidgets_->set();
}

void GraphicManager::sendMessage(
    MessageType type, std::shared_ptr<Companion> companion, const std::string& text)
{
    getManager()->sendMessage(type, companion, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString& text)
{
    mainWindow_->addTextToAppLogWidget(text);
}

std::size_t GraphicManager::getCompanionPanelChildrenSize()
{
    return mainWindow_->getCompanionPanelChildrenSize();
}

void GraphicManager::hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group)
{
    group->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group)
{
    group->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, std::shared_ptr<QWidget> widget)
{
    mainWindow_->addWidgetToContainerAndSetParentTo(position, widget);
}

void GraphicManager::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
{
    mainWindow_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget)
{
    mainWindow_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::createTextDialogAndShow(
    std::shared_ptr<QWidget> parent, DialogType dialogType, const std::string& text,
    std::shared_ptr<std::vector<ButtonInfo>> buttonInfo)
{
    // TODO delete objects for closed dialoges?
    auto dialog = std::make_shared<TextDialog>(parent, dialogType, text, buttonInfo);

    dialog->set();
    dialog->show();
}

void GraphicManager::createCompanion()
{
    auto action = std::make_shared<CompanionAction>(ChatActionType::CREATE, nullptr);
    action->set();
}

void GraphicManager::createGroupChat()
{
    auto action = std::make_shared<GroupChatAction>(ChatActionType::CREATE);
    action->set();
}

void GraphicManager::updateCompanion(std::shared_ptr<Companion> companion)
{
    auto action = std::make_shared<CompanionAction>(ChatActionType::UPDATE, companion);
    action->set();
}

void GraphicManager::clearCompanionHistory(std::shared_ptr<Companion> companion)
{
    auto action = std::make_shared<CompanionAction>(ChatActionType::CLEAR_HISTORY, companion);
    action->set();
}

void GraphicManager::clearChatHistory(std::shared_ptr<WidgetGroup> widgetGroup)
{
    widgetGroup->clearChatHistory();
}

void GraphicManager::deleteCompanion(std::shared_ptr<Companion> companion)
{
    auto action = std::make_shared<CompanionAction>(ChatActionType::DELETE, companion);
    action->set();
}

void GraphicManager::sendCompanionDataToManager(std::shared_ptr<CompanionAction> action)
{
    switch (action->getActionType()) {
    case ChatActionType::CREATE:
        getManager()->createCompanion(action);

        break;

    case ChatActionType::UPDATE:
        getManager()->updateCompanion(action);

        break;

    case ChatActionType::DELETE:
        getManager()->deleteCompanion(action);

        break;

    case ChatActionType::CLEAR_HISTORY:
        getManager()->clearCompanionHistory(action);

        break;
    }
}

void GraphicManager::showCompanionInfoDialog(
    std::shared_ptr<CompanionAction> companionAction, std::string&& header)
{
    std::shared_ptr<QWidget> parent = nullptr;
    // void (TextDialog::*function)() = nullptr;
    std::function<void(TextDialog &)> function;
    // void (QDialog::*function)() = nullptr;

    auto formDialog = companionAction->getDialog();

    if (formDialog) {
        parent = formDialog;
        function = [](TextDialog &dialog) { dialog.closeSelfAndParentDialog(); };
    }
    else {
        // function = &TextDialog::closeSelf;
        function = [](TextDialog &dialog) { dialog.closeSelf(); };
    }

    createTextDialogAndShow(
        parent,
        DialogType::INFO,
        buildDialogText(
            std::move(header),
            std::vector<std::string> {
                std::string("name: ") + companionAction->getName(),
                std::string("ipAddress: ") + companionAction->getIpAddress(),
                std::string("port: ") + companionAction->getClientPort()
            }),
        createOkButtonInfoVector(function));
}

void GraphicManager::sendNewPasswordDataToManager(std::shared_ptr<PasswordAction> action)
{
    getManager()->createUserPassword(action);
}

void GraphicManager::sendExistingPasswordDataToManager(std::shared_ptr<PasswordAction> action)
{
    getManager()->authenticateUser(action);
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
    auto action = std::make_shared<PasswordAction>(PasswordActionType::CREATE);
    action->set();
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
    auto action = std::make_shared<PasswordAction>(PasswordActionType::GET);
    action->set();
}

void GraphicManager::markMessageWidgetAsSent(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(messageToMessageWidgetMapMutex_);

        try {
            companion->getMappedMessageWidgetByMessage(message)->setMessageWidgetAsSent();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::markMessageWidgetAsReceived(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(messageToMessageWidgetMapMutex_);

        try {
            companion->getMappedMessageWidgetByMessage(message)->setMessageWidgetAsReceived();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup> group)
{
    group->sortChatHistoryElements();
}

void GraphicManager::sendFile(std::shared_ptr<Companion> companion)
{
    auto action = std::make_shared<FileAction>(FileActionType::SEND, "", companion);
    action->set();
}

void GraphicManager::saveFile(const std::string& networkId, std::shared_ptr<Companion> companion)
{
    auto action = std::make_shared<FileAction>(FileActionType::SAVE, networkId, companion);
    action->set();
}

std::shared_ptr<GraphicManager> getGraphicManager()
{
    QCoreApplication *coreApp = QCoreApplication::instance();
    ChatApp *app = dynamic_cast<ChatApp*>(coreApp);

    if (!app)
        return nullptr;

    return app->graphicManager_;
}
