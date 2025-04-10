#include "action.hpp"

void Action::set() {
    this->dialogPtr_->setAction(this);
    this->dialogPtr_->set();

    // if(this->dialogPtr_->getContainsDialogPtr())

    FileAction* actionCastPtr = dynamic_cast<FileAction*>(this);

    if(actionCastPtr) {
        if(actionCastPtr->getType() == FileActionType::SEND) {
            actionCastPtr->dialogPtr_->showDialog();
        }
        else if(actionCastPtr->getType() == FileActionType::SAVE) {
            actionCastPtr->defineFilePath();
        }
        else {
            logArgsError("unknown action type");
        }
    }
    else {
        this->dialogPtr_->show();
    }
}

Dialog* Action::getDialogPtr() {
    return this->dialogPtr_;
}

CompanionAction::CompanionAction(
    ChatActionType actionType, Companion* companionPtr) :
    actionType_(actionType), companionPtr_(companionPtr),
    dataPtr_(nullptr), Action(nullptr) {

    MainWindow* mainWindowPtr = getGraphicManagerPtr()->getMainWindowPtr();

    switch(actionType) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
        dialogPtr_ = new CompanionDataDialog(actionType_, mainWindowPtr, companionPtr_);

        break;

    case ChatActionType::DELETE:
        dialogPtr_ = new TextDialog(
            mainWindowPtr,
            DialogType::WARNING,
            deleteCompanionDialogText,
            getButtonInfoVectorPtr(deleteCompanionButtonText));

        break;

    case ChatActionType::CLEAR_HISTORY:
        dialogPtr_ = new TextDialog(
            mainWindowPtr,
            DialogType::WARNING,
            clearCompanionHistoryDialogText,
            getButtonInfoVectorPtr(clearHistoryButtonText));

        break;

    case ChatActionType::SEND_HISTORY:
        dialogPtr_ = new TextDialog(
            mainWindowPtr,
            DialogType::WARNING,
            // getArgumentedQString(
            //     sendChatHistoryToCompanionDialogText, companionPtr->getName()),
            std::format(sendChatHistoryToCompanionDialogText, companionPtr->getName()),
            getButtonInfoVectorPtr(sendChatHistoryButtonText));

        break;
    }
}

CompanionAction::~CompanionAction() {
    delete this->dataPtr_;
}

ChatActionType CompanionAction::getActionType() const {
    return this->actionType_;
}

std::string CompanionAction::getName() const {
    return this->dataPtr_->getName();
}

std::string CompanionAction::getIpAddress() const {
    return this->dataPtr_->getIpAddress();
}

std::string CompanionAction::getServerPort() const {
    return this->dataPtr_->getServerPort();
}

std::string CompanionAction::getClientPort() const {
    return this->dataPtr_->getClientPort();
}

int CompanionAction::getCompanionId() const {
    return this->companionPtr_->getId();
}

Companion* CompanionAction::getCompanionPtr() const {
    return this->companionPtr_;
}

void CompanionAction::updateCompanionObjectData() {
    this->companionPtr_->updateData(this->dataPtr_);
}

// TODO deletion of action objects
void CompanionAction::sendData() {
    if(this->actionType_ == ChatActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManagerPtr()->sendChatHistoryToCompanion(this->companionPtr_);
        return;
    }

    std::string name;
    std::string ipAddress;
    std::string serverPort { "" };
    std::string clientPort;

    switch(this->actionType_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE: {
            CompanionDataDialog* dataDialogPtr =
                dynamic_cast<CompanionDataDialog*>(this->dialogPtr_);

            name = dataDialogPtr->getNameString();
            ipAddress = dataDialogPtr->getIpAddressString();
            clientPort = dataDialogPtr->getPortString();
        }

        break;

    case ChatActionType::DELETE:
    case ChatActionType::CLEAR_HISTORY: {
            name = this->companionPtr_->getName();
            ipAddress = this->companionPtr_->getSocketIpAddress();
            clientPort = std::to_string(this->companionPtr_->getSocketClientPort());
        }

        break;
    }

    logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    this->dataPtr_ = new CompanionData(name, ipAddress, serverPort, clientPort);

    getGraphicManagerPtr()->sendCompanionDataToManager(this);
}

GroupChatAction::GroupChatAction(ChatActionType actionType)
    : actionType_(actionType), dataPtr_(new GroupChatData), Action(nullptr) {

    MainWindow* mainWindowPtr = getGraphicManagerPtr()->getMainWindowPtr();

    switch(actionType) {
    case ChatActionType::CREATE:
        dialogPtr_ = new GroupChatDataDialog(actionType_, mainWindowPtr);

        break;
    }
}

GroupChatAction::~GroupChatAction() {
    delete this->dataPtr_;
}

void GroupChatAction::sendData() {

}

PasswordAction::PasswordAction(PasswordActionType actionType) : Action(nullptr) {
    actionType_ = actionType;

    switch(actionType) {
    case PasswordActionType::CREATE:
        dialogPtr_ = new CreatePasswordDialog;

        break;

    case PasswordActionType::GET:
        dialogPtr_ = new GetPasswordDialog;

        break;
    }
}

PasswordAction::~PasswordAction() {
    if(this->actionType_ == PasswordActionType::GET) {
        this->dialogPtr_->close();
    }
}

std::string PasswordAction::getPassword() {
    return *this->passwordPtr_;
}

void PasswordAction::sendData() {
    switch(this->actionType_) {
    case PasswordActionType::CREATE: {
            CreatePasswordDialog* passwordDialogPtr =
                dynamic_cast<CreatePasswordDialog*>(this->dialogPtr_);

            auto text1 = passwordDialogPtr->getFirstEditText();
            auto text2 = passwordDialogPtr->getSecondEditText();

            if(text1 == text2) {
                if(text1.size() == 0) {
                    showErrorDialogAndLogError(
                        "Empty password is invalid", this->getDialogPtr());

                    return;
                }

                this->passwordPtr_ = &text1;
                getGraphicManagerPtr()->sendNewPasswordDataToManager(this);
            }
            else {
                showErrorDialogAndLogError(
                    "Entered passwords are not equal", this->getDialogPtr());
            }
        }

        break;

    case PasswordActionType::GET: {
            GetPasswordDialog* passwordDialogPtr =
                dynamic_cast<GetPasswordDialog*>(this->dialogPtr_);

            auto text = passwordDialogPtr->getEditText();

            if(text.size() == 0) {
                showErrorDialogAndLogError(
                    "Empty password is invalid", this->getDialogPtr());

                return;
            }

            this->passwordPtr_ = &text;
            getGraphicManagerPtr()->sendExistingPasswordDataToManager(this);
        }

        break;
    }
}

FileAction::FileAction(
    FileActionType actionType, const std::string& networkId, Companion* companionPtr) :
    Action(nullptr) {
    actionType_ = actionType;
    companionPtr_ = companionPtr;
    networkId_ = networkId;

    QString windowTitle = getConstantMappingValue(
        "fileDialogTypeQStringRepresentation",
        &fileDialogTypeQStringRepresentation,
        actionType);

    dialogPtr_ = new FileDialog(this, windowTitle);
}

FileActionType FileAction::getType() const {
    return this->actionType_;
}

Companion* FileAction::getCompanionPtr() const {
    return this->companionPtr_;
}

std::filesystem::path FileAction::getPath() const {
    return this->filePath_;
}

void FileAction::sendData() {
    logArgs("FileAction::sendData");

    auto dialogPtr = dynamic_cast<FileDialog*>(this->dialogPtr_)->getFileDialogPtr();

    switch(this->actionType_) {
    case FileActionType::SEND: {
            for(auto& pathQString : dialogPtr->selectedFiles()) {  // one file
                logArgs(pathQString);

                auto path = std::filesystem::path(pathQString.toStdString());

                this->filePath_ = path;  // TODO ???

                getManagerPtr()->sendMessage(
                    MessageType::FILE, this->getCompanionPtr(), this,
                    std::format("SEND FILE: {}", this->filePath_.filename().string()));

                getManagerPtr()->setLastOpenedPath(path.parent_path());
            }
        }

        break;

    case FileActionType::SAVE: {
            // for(auto& pathQString : dialogPtr->selectedFiles())  // one file
            // {
            //     logArgs(pathQString);

            //     auto path = std::filesystem::path(pathQString.toStdString());

            //     this->filePath_ = path;

            //     // set file path for file operator
            //     this->companionPtr_->getFileOperatorStoragePtr()->
            //         getOperator(this->networkId_)->setFilePath(path);

            //     // send without saving to db
            //     bool result = this->companionPtr_->sendMessage(
            //         false, NetworkMessageType::FILE_REQUEST,
            //         this->networkId_, nullptr);

            //     getManagerPtr()->setLastOpenedPath(path.parent_path());

            // }

            // set file path for file operator
            bool setResult = this->companionPtr_->setFileOperatorFilePath(
                this->networkId_, this->filePath_);

            if(setResult) {
                // send without saving to db
                bool result = this->companionPtr_->sendMessage(
                    false, NetworkMessageType::FILE_REQUEST,
                    this->networkId_, nullptr);

                getManagerPtr()->setLastOpenedPath(this->filePath_.parent_path());
            }
            else {
                logArgsErrorWithTemplate(
                    "error saving file, path: {}", this->filePath_.string());
            }
        }

        break;
    }
}

void FileAction::defineFilePath() {
    this->filePath_ = QFileDialog::getSaveFileName(
        getGraphicManagerPtr()->getMainWindowPtr(),
        "Save File",
        // getQString(getManagerPtr()->getLastOpenedPath().string())).toStdString();
        getQString(getManagerPtr()->getLastOpenedPath().string())).toStdString();

    this->sendData();
}
