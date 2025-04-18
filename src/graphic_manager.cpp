#include "graphic_manager.hpp"

GraphicManager::GraphicManager() :
    messageToMessageWidgetMapMutex_(std::mutex()) {
    // mapMessageToMessageWidget_(std::map<const Message*, const MessageWidget*>()) {
    // stubWidgetsPtr_ = new StubWidgetGroup;
    // mainWindowPtr_ = new MainWindow;

    // stubWidgetsPtr_ = nullptr;
    // mainWindowPtr_ = nullptr;
}

MainWindow* GraphicManager::getMainWindowPtr() {
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
    QWidget* leftContainerPtr, QWidget* centralContainerPtr) {
    this->stubWidgetsPtr_->setParents(leftContainerPtr, centralContainerPtr);
}

void GraphicManager::setStubWidgets() {
    this->stubWidgetsPtr_->set();
}

void GraphicManager::sendMessage(
    MessageType type, Companion* companionPtr, const std::string& text) {
    getManagerPtr()->sendMessage(type, companionPtr, nullptr, text);
}

void GraphicManager::addTextToAppLogWidget(const QString& text) {
    this->mainWindowPtr_->addTextToAppLogWidget(text);
}

std::size_t GraphicManager::getCompanionPanelChildrenSize() {
    return this->mainWindowPtr_->getCompanionPanelChildrenSize();
}

void GraphicManager::hideWidgetGroupCentralPanel(WidgetGroup* groupPtr) {
    groupPtr->hideCentralPanel();
}

void GraphicManager::showWidgetGroupCentralPanel(WidgetGroup* groupPtr) {
    groupPtr->showCentralPanel();
}

void GraphicManager::addWidgetToMainWindowContainerAndSetParentTo(
    MainWindowContainerPosition position, QWidget* widgetPtr) {
    this->mainWindowPtr_->addWidgetToContainerAndSetParentTo(
        position, widgetPtr);
}

void GraphicManager::addWidgetToCompanionPanel(SocketInfoBaseWidget* widget) {
    this->mainWindowPtr_->addWidgetToCompanionPanel(widget);
}

void GraphicManager::removeWidgetFromCompanionPanel(SocketInfoBaseWidget* widget) {
    this->mainWindowPtr_->removeWidgetFromCompanionPanel(widget);
}

void GraphicManager::createTextDialogAndShow(
    QWidget* parentPtr, DialogType dialogType, const std::string& text,
    std::vector<ButtonInfo>* buttonInfoPtr) {
    // TODO delete objects for closed dialoges?
    TextDialog* dialogPtr = new TextDialog(parentPtr, dialogType, text, buttonInfoPtr);

    dialogPtr->set();
    dialogPtr->show();
}

void GraphicManager::createCompanion() {
    CompanionAction* actionPtr = new CompanionAction(
        ChatActionType::CREATE, nullptr);

    actionPtr->set();
}

void GraphicManager::createGroupChat() {
    GroupChatAction* actionPtr = new GroupChatAction(ChatActionType::CREATE);
    actionPtr->set();
}

void GraphicManager::updateCompanion(Companion* companionPtr) {
    CompanionAction* actionPtr = new CompanionAction(
        ChatActionType::UPDATE, companionPtr);

    actionPtr->set();
}

void GraphicManager::clearCompanionHistory(Companion* companionPtr) {
    CompanionAction* actionPtr = new CompanionAction(
        ChatActionType::CLEAR_HISTORY, companionPtr);

    actionPtr->set();
}

void GraphicManager::clearChatHistory(WidgetGroup* widgetGroupPtr) {
    widgetGroupPtr->clearChatHistory();
}

void GraphicManager::deleteCompanion(Companion* companionPtr) {
    CompanionAction* actionPtr = new CompanionAction(
        ChatActionType::DELETE, companionPtr);

    actionPtr->set();
}

void GraphicManager::sendCompanionDataToManager(CompanionAction* actionPtr) {
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
    CompanionAction* companionActionPtr, std::string&& header) {
    QWidget* parentPtr = nullptr;
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

void GraphicManager::sendNewPasswordDataToManager(PasswordAction* actionPtr) {
    getManagerPtr()->createUserPassword(actionPtr);
}

void GraphicManager::sendExistingPasswordDataToManager(PasswordAction* actionPtr) {
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
    PasswordAction* actionPtr = new PasswordAction(PasswordActionType::CREATE);
    actionPtr->set();
}

void GraphicManager::enableMainWindowBlurEffect() {
    this->mainWindowPtr_->enableBlurEffect();
}

void GraphicManager::disableMainWindowBlurEffect() {
    this->mainWindowPtr_->disableBlurEffect();
}

void GraphicManager::getEntrancePassword() {
    PasswordAction* actionPtr = new PasswordAction(PasswordActionType::GET);
    actionPtr->set();
}

void GraphicManager::markMessageWidgetAsSent(
    Companion* companionPtr, const Message* messagePtr) {
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
    Companion* companionPtr, const Message* messagePtr) {
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

void GraphicManager::sortChatHistoryElementsForWidgetGroup(WidgetGroup* groupPtr) {
    groupPtr->sortChatHistoryElements();
}

void GraphicManager::sendFile(Companion* companionPtr) {
    FileAction* actionPtr = new FileAction(FileActionType::SEND, "", companionPtr);
    actionPtr->set();
}

void GraphicManager::saveFile(const std::string& networkId, Companion* companionPtr) {
    FileAction* actionPtr = new FileAction(FileActionType::SAVE, networkId, companionPtr);
    actionPtr->set();
}

GraphicManager* getGraphicManagerPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->graphicManagerPtr_;
}
