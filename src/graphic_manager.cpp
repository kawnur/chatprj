#include "graphic_manager.hpp"

GraphicManager::GraphicManager() :
    messageToMessageWidgetMapMutex_(std::mutex()) {
    // mapMessageToMessageWidget_(std::map<std::shared_ptr<Message>, std::shared_ptr<MessageWidget>>()) {
    // stubWidgetsPtr_ = new StubWidgetGroup;
    // mainWindowPtr_ = new MainWindow;

    // stubWidgetsPtr_ = nullptr;
    // mainWindowPtr_ = nullptr;
}

std::shared_ptr<MainWindow> GraphicManager::getMainWindowPtr() {
    return this->mainWindowPtr_;
}

void GraphicManager::set() {
    stubWidgetsPtr_ = new StubWidgetGroup;
    mainWindowPtr_ = new MainWindow;

    // this->stubWidgetsPtr_->set();
    this->mainWindowPtr_->set();
    this->mainWindowPtr_->show();
}

void GraphicManager::setParentsForStubs(
    std::shared_ptr<QWidget> leftContainerPtr, std::shared_ptr<QWidget> centralContainerPtr) {
    this->stubWidgetsPtr_->setParents(leftContainerPtr, centralContainerPtr);
}

void GraphicManager::setStubWidgets() {
    this->stubWidgetsPtr_->set();
}

void GraphicManager::sendMessage(
    MessageType type, std::shared_ptr<Companion> companionPtr, const std::string& text) {
    getManagerPtr()->sendMessage(type, companionPtr, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString& text) {
    this->mainWindowPtr_->addTextToAppLogWidget(text);
}

std::size_t GraphicManager::getCompanionPanelChildrenSize() {
    return this->mainWindowPtr_->getCompanionPanelChildrenSize();
}

void GraphicManager::hideWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> groupPtr) {
    groupPtr->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(std::shared_ptr<WidgetGroup> groupPtr) {
    groupPtr->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, std::shared_ptr<QWidget> widgetPtr) {
    this->mainWindowPtr_->addWidgetToContainerAndSetParentTo(
        position, widgetPtr);
}

void GraphicManager::addWidgetToCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->mainWindowPtr_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::removeWidgetFromCompanionPanel(std::shared_ptr<SocketInfoBaseWidget> widget) {
    this->mainWindowPtr_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::createTextDialogAndShow(
    std::shared_ptr<QWidget> parentPtr, DialogType dialogType, const std::string& text,
    std::vector<ButtonInfo>* buttonInfoPtr) {
    // TODO delete objects for closed dialoges?
    std::shared_ptr<TextDialog> dialogPtr = new TextDialog(parentPtr, dialogType, text, buttonInfoPtr);

    dialogPtr->set();
    dialogPtr->show();
}

void GraphicManager::createCompanion() {
    std::shared_ptr<CompanionAction> actionPtr = new CompanionAction(
        ChatActionType::CREATE, nullptr);

    actionPtr->set();
}

void GraphicManager::createGroupChat() {
    std::shared_ptr<GroupChatAction> actionPtr = new GroupChatAction(ChatActionType::CREATE);
    actionPtr->set();
}

void GraphicManager::updateCompanion(std::shared_ptr<Companion> companionPtr) {
    std::shared_ptr<CompanionAction> actionPtr = new CompanionAction(
        ChatActionType::UPDATE, companionPtr);

    actionPtr->set();
}

void GraphicManager::clearCompanionHistory(std::shared_ptr<Companion> companionPtr) {
    std::shared_ptr<CompanionAction> actionPtr = new CompanionAction(
        ChatActionType::CLEAR_HISTORY, companionPtr);

    actionPtr->set();
}

void GraphicManager::clearChatHistory(std::shared_ptr<WidgetGroup> widgetGroupPtr) {
    widgetGroupPtr->clearChatHistory();
}

void GraphicManager::deleteCompanion(std::shared_ptr<Companion> companionPtr) {
    std::shared_ptr<CompanionAction> actionPtr = new CompanionAction(
        ChatActionType::DELETE, companionPtr);

    actionPtr->set();
}

void GraphicManager::sendCompanionDataToManager(std::shared_ptr<CompanionAction> actionPtr) {
    switch(actionPtr->getActionType()) {
    case ChatActionType::CREATE:
        getManagerPtr()->createCompanion(actionPtr);

        break;

    case ChatActionType::UPDATE:
        getManagerPtr()->updateCompanion(actionPtr);

        break;

    case ChatActionType::DELETE:
        getManagerPtr()->deleteCompanion(actionPtr);

        break;

    case ChatActionType::CLEAR_HISTORY:
        getManagerPtr()->clearCompanionHistory(actionPtr);

        break;
    }
}

void GraphicManager::showCompanionInfoDialog(
    std::shared_ptr<CompanionAction> companionActionPtr, std::string&& header) {
    std::shared_ptr<QWidget> parentPtr = nullptr;
    void (TextDialog::*functionPtr)() = nullptr;
    // void (QDialog::*functionPtr)() = nullptr;

    auto formDialogPtr = companionActionPtr->getDialogPtr();

    if(formDialogPtr) {
        parentPtr = formDialogPtr;
        functionPtr = &TextDialog::closeSelfAndParentDialog;
    }
    else {
        functionPtr = &TextDialog::closeSelf;
    }

    this->createTextDialogAndShow(
        parentPtr,
        DialogType::INFO,
        buildDialogText(
            std::move(header),
            std::vector<std::string> {
                std::string("name: ") + companionActionPtr->getName(),
                std::string("ipAddress: ") + companionActionPtr->getIpAddress(),
                std::string("port: ") + companionActionPtr->getClientPort()
            }),
        createOkButtonInfoVector(functionPtr));

    delete companionActionPtr;
}

void GraphicManager::sendNewPasswordDataToManager(std::shared_ptr<PasswordAction> actionPtr) {
    getManagerPtr()->createUserPassword(actionPtr);
}

void GraphicManager::sendExistingPasswordDataToManager(std::shared_ptr<PasswordAction> actionPtr) {
    getManagerPtr()->authenticateUser(actionPtr);
}

void GraphicManager::hideCompanionPanelStub() {
    this->stubWidgetsPtr_->hideSocketInfoStubWidget();
}

void GraphicManager::hideCentralPanelStub() {
    this->stubWidgetsPtr_->hideCentralPanel();
}

void GraphicManager::showCentralPanelStub() {
    this->stubWidgetsPtr_->showCentralPanel();
}

void GraphicManager::hideInfoViaBlur() {
    this->enableMainWindowBlurEffect();
}

void GraphicManager::showInfoViaBlur() {
    this->disableMainWindowBlurEffect();
}

void GraphicManager::hideInfoViaStubs() {
    this->stubWidgetsPtr_->setLeftPanelWidth(
        this->mainWindowPtr_->getLeftPanelWidgetWidth());

    this->mainWindowPtr_->hideLeftAndRightPanels();
    getManagerPtr()->hideSelectedCompanionCentralPanel();
    this->stubWidgetsPtr_->showStubPanels();
}

void GraphicManager::showInfoViaStubs() {
    this->mainWindowPtr_->showLeftAndRightPanels();
    getManagerPtr()->showSelectedCompanionCentralPanel();
    this->stubWidgetsPtr_->hideStubPanels();
}

void GraphicManager::hideInfo() {
    this->hideInfoViaBlur();
    // this->hideInfoViaStubs();
}

void GraphicManager::showInfo() {
    getManagerPtr()->startUserAuthentication();

    // this->showInfoViaBlur();
    // this->showInfoViaStubs();
}

void GraphicManager::createEntrancePassword() {
    std::shared_ptr<PasswordAction> actionPtr = new PasswordAction(PasswordActionType::CREATE);
    actionPtr->set();
}

void GraphicManager::enableMainWindowBlurEffect() {
    this->mainWindowPtr_->enableBlurEffect();
}

void GraphicManager::disableMainWindowBlurEffect() {
    this->mainWindowPtr_->disableBlurEffect();
}

void GraphicManager::getEntrancePassword() {
    std::shared_ptr<PasswordAction> actionPtr = new PasswordAction(PasswordActionType::GET);
    actionPtr->set();
}

void GraphicManager::markMessageWidgetAsSent(
    std::shared_ptr<Companion> companionPtr, std::shared_ptr<Message> messagePtr) {
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try {
            companionPtr->getMappedMessageWidgetPtrByMessagePtr(messagePtr)->
                setMessageWidgetAsSent();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::markMessageWidgetAsReceived(
    std::shared_ptr<Companion> companionPtr, std::shared_ptr<Message> messagePtr) {
    auto setLambda = [&, this](){
        std::lock_guard<std::mutex> lock(this->messageToMessageWidgetMapMutex_);

        try {
            companionPtr->getMappedMessageWidgetPtrByMessagePtr(messagePtr)->
                setMessageWidgetAsReceived();
        }
        catch(std::out_of_range) {
            return;
        }
    };

    runAndLogException(setLambda);
}

void GraphicManager::sortChatHistoryElementsForWidgetGroup(std::shared_ptr<WidgetGroup> groupPtr) {
    groupPtr->sortChatHistoryElements();
}

void GraphicManager::sendFile(std::shared_ptr<Companion> companionPtr) {
    std::shared_ptr<FileAction> actionPtr = new FileAction(FileActionType::SEND, "", companionPtr);
    actionPtr->set();
}

void GraphicManager::saveFile(const std::string& networkId, std::shared_ptr<Companion> companionPtr) {
    std::shared_ptr<FileAction> actionPtr = new FileAction(FileActionType::SAVE, networkId, companionPtr);
    actionPtr->set();
}

std::shared_ptr<GraphicManager> getGraphicManagerPtr() {
    std::shared_ptr<QCoreApplication> coreApp = QCoreApplication::instance();
    std::shared_ptr<ChatApp> app = dynamic_cast<std::shared_ptr<ChatApp>>(coreApp);
    return app->graphicManagerPtr_;
}
