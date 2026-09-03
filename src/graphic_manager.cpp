#include "graphic_manager.hpp"

GraphicManager::GraphicManager() :
    messageToMessageWidgetMapMutex_(std::mutex()) {
    // mapMessageToMessageWidget_(std::map<std::shared_ptr<Message>, std::shared_ptr<MessageWidget>>()) {
    // stubWidgets_ = new StubWidgetGroup;
    // mainWindow_ = new MainWindow;

    // stubWidgets_ = nullptr;
    // mainWindow_ = nullptr;
}

std::shared_ptr<MainWindow> GraphicManager::getMainWindow() {
    return this->mainWindow_;
}

void GraphicManager::set() {
    stubWidgets_ = new StubWidgetGroup;
    mainWindow_ = new MainWindow;

    // this->stubWidgets_->set();
    this->mainWindow_->set();
    this->mainWindow_->show();
}

void GraphicManager::setParentsForStubs(
    std::shared_ptr<QWidget> leftContainer, std::shared_ptr<QWidget> centralContainer) {
    this->stubWidgets_->setParents(leftContainer, centralContainer);
}

void GraphicManager::setStubWidgets() {
    this->stubWidgets_->set();
}

void GraphicManager::sendMessage(
    MessageType type, std::shared_ptr<Companion> companion, const std::string& text) {
    getManager()->sendMessage(type, companion, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString& text) {
    this->mainWindow_->addTextToAppLogWidget(text);
}

std::size_t GraphicManager::getCompanionPanelChildrenSize() {
    return this->mainWindow_->getCompanionPanelChildrenSize();
}

void GraphicManager::hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group) {
    group->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> group) {
    group->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, std::shared_ptr<QWidget> widget) {
    this->mainWindow_->addWidgetToContainerAndSetParentTo(
        position, widget);
}

void GraphicManager::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->mainWindow_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->mainWindow_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::createTextDialogAndShow(
    std::shared_ptr<QWidget> parent, DialogType dialogType, const std::string& text,
    std::vector<ButtonInfo>* buttonInfo) {
    // TODO delete objects for closed dialoges?
    std::shared_ptr<TextDialog> dialog = new TextDialog(parent, dialogType, text, buttonInfo);

    dialog->set();
    dialog->show();
}

void GraphicManager::createCompanion() {
    std::shared_ptr<CompanionAction> action = new CompanionAction(
        ChatActionType::CREATE, nullptr);

    action->set();
}

void GraphicManager::createGroupChat() {
    std::shared_ptr<GroupChatAction> action = new GroupChatAction(ChatActionType::CREATE);
    action->set();
}

void GraphicManager::updateCompanion(std::shared_ptr<Companion> companion) {
    std::shared_ptr<CompanionAction> action = new CompanionAction(
        ChatActionType::UPDATE, companion);

    action->set();
}

void GraphicManager::clearCompanionHistory(std::shared_ptr<Companion> companion) {
    std::shared_ptr<CompanionAction> action = new CompanionAction(
        ChatActionType::CLEAR_HISTORY, companion);

    action->set();
}

void GraphicManager::clearChatHistory(std::shared_ptr<WidgetGroup> widgetGroup) {
    widgetGroup->clearChatHistory();
}

void GraphicManager::deleteCompanion(std::shared_ptr<Companion> companion) {
    std::shared_ptr<CompanionAction> action = new CompanionAction(
        ChatActionType::DELETE, companion);

    action->set();
}

void GraphicManager::sendCompanionDataToManager(std::shared_ptr<CompanionAction> action) {
    switch(action->getActionType()) {
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
    void (TextDialog::*function)() = nullptr;
    // void (QDialog::*function)() = nullptr;

    auto formDialog = companionAction->getDialog();

    if(formDialog) {
        parent = formDialog;
        function = &TextDialog::closeSelfAndParentDialog;
    }
    else {
        function = &TextDialog::closeSelf;
    }

    this->createTextDialogAndShow(
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

void GraphicManager::sendNewPasswordDataToManager(std::shared_ptr<PasswordAction> action) {
    getManager()->createUserPassword(action);
}

void GraphicManager::sendExistingPasswordDataToManager(std::shared_ptr<PasswordAction> action) {
    getManager()->authenticateUser(action);
}

void GraphicManager::hideCompanionPanelStub() {
    this->stubWidgets_->hideSocketInfoStubWidget();
}

void GraphicManager::hideCentralPanelStub() {
    this->stubWidgets_->hideCentralPanel();
}

void GraphicManager::showCentralPanelStub() {
    this->stubWidgets_->showCentralPanel();
}

void GraphicManager::hideInfoViaBlur() {
    this->enableMainWindowBlurEffect();
}

void GraphicManager::showInfoViaBlur() {
    this->disableMainWindowBlurEffect();
}

void GraphicManager::hideInfoViaStubs() {
    this->stubWidgets_->setLeftPanelWidth(
        this->mainWindow_->getLeftPanelWidgetWidth());

    this->mainWindow_->hideLeftAndRightPanels();
    getManager()->hideSelectedCompanionCentralPanel();
    this->stubWidgets_->showStubPanels();
}

void GraphicManager::showInfoViaStubs() {
    this->mainWindow_->showLeftAndRightPanels();
    getManager()->showSelectedCompanionCentralPanel();
    this->stubWidgets_->hideStubPanels();
}

void GraphicManager::hideInfo() {
    this->hideInfoViaBlur();
    // this->hideInfoViaStubs();
}

void GraphicManager::showInfo() {
    getManager()->startUserAuthentication();

    // this->showInfoViaBlur();
    // this->showInfoViaStubs();
}

void GraphicManager::createEntrancePassword() {
    std::shared_ptr<PasswordAction> action = new PasswordAction(PasswordActionType::CREATE);
    action->set();
}

void GraphicManager::enableMainWindowBlurEffect() {
    this->mainWindow_->enableBlurEffect();
}

void GraphicManager::disableMainWindowBlurEffect() {
    this->mainWindow_->disableBlurEffect();
}

void GraphicManager::getEntrancePassword() {
    std::shared_ptr<PasswordAction> action = new PasswordAction(PasswordActionType::GET);
    action->set();
}

void GraphicManager::markMessageWidgetAsSent(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message) {
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try {
            companion->getMappedMessageWidgetByMessage(message)->
                setMessageWidgetAsSent();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::markMessageWidgetAsReceived(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message) {
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try {
            companion->getMappedMessageWidgetByMessage(message)->
                setMessageWidgetAsReceived();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup> group) {
    group->sortChatHistoryElements();
}

void GraphicManager::sendFile(std::shared_ptr<Companion> companion) {
    std::shared_ptr<FileAction> action = new FileAction(FileActionType::SEND, "", companion);
    action->set();
}

void GraphicManager::saveFile(const std::string& networkId, std::shared_ptr<Companion> companion) {
    std::shared_ptr<FileAction> action = new FileAction(FileActionType::SAVE, networkId, companion);
    action->set();
}

std::shared_ptr<GraphicManager> getGraphicManager() {
    std::shared_ptr<QCoreApplication> coreApp = QCoreApplication::instance();
    std::shared_ptr<ChatApp> app = dynamic_cast<std::shared_ptr<ChatApp>>(coreApp);
    return app->graphicManager_;
}
