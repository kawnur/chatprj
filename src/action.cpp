#include "action.hpp"

void Action::set() {
    this->dialog_->setAction(shared_from_this());
    this->dialog_->set();

    // if(this->dialog_->getContainsDialog())

    auto actionCast = dynamic_pointer_cast<FileAction>(shared_from_this());

    if (!actionCast) {
        this->dialog_->show();

        return;
    }

    switch(actionCast->getType()) {
    case FileActionType::SEND:
        actionCast->dialog_->showDialog();
        break;
    case FileActionType::SAVE:
        actionCast->defineFilePath();
        break;
    default:
        logArgsError("unknown action type");
        break;
    }
}

std::shared_ptr<Dialog> Action::getDialog() {
    return this->dialog_;
}

CompanionAction::CompanionAction(
    ChatActionType actionType, std::shared_ptr<Companion> companion) :
    actionType_(actionType), companion_(companion),
    data_(nullptr), Action(nullptr) {

    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch(actionType) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
        dialog_ = std::make_shared<CompanionDataDialog>(actionType_, mainWindow, companion_);

        break;

    case ChatActionType::DELETE:
        dialog_ = std::make_shared<TextDialog>(
            mainWindow,
            DialogType::WARNING,
            deleteCompanionDialogText,
            getButtonInfoVector(deleteCompanionButtonText));

        break;

    case ChatActionType::CLEAR_HISTORY:
        dialog_ = std::make_shared<TextDialog>(
            mainWindow,
            DialogType::WARNING,
            clearCompanionHistoryDialogText,
            getButtonInfoVector(clearHistoryButtonText));

        break;

    case ChatActionType::SEND_HISTORY:
        dialog_ = std::make_shared<TextDialog>(
            mainWindow,
            DialogType::WARNING,
            // getArgumentedQString(
            //     sendChatHistoryToCompanionDialogText, companion->getName()),
            std::format(sendChatHistoryToCompanionDialogText, companion->getName()),
            getButtonInfoVector(sendChatHistoryButtonText));

        break;
    }
}

ChatActionType CompanionAction::getActionType() const {
    return this->actionType_;
}

std::string CompanionAction::getName() const {
    return this->data_->getName();
}

std::string CompanionAction::getIpAddress() const {
    return this->data_->getIpAddress();
}

std::string CompanionAction::getServerPort() const {
    return this->data_->getServerPort();
}

std::string CompanionAction::getClientPort() const {
    return this->data_->getClientPort();
}

int CompanionAction::getCompanionId() const {
    return this->companion_->getId();
}

std::shared_ptr<Companion> CompanionAction::getCompanion() const {
    return this->companion_;
}

void CompanionAction::updateCompanionObjectData() {
    this->companion_->updateData(this->data_);
}

// TODO deletion of action objects
void CompanionAction::sendData() {
    if(this->actionType_ == ChatActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(this->companion_);
        return;
    }

    std::string name;
    std::string ipAddress;
    std::string serverPort { "" };
    std::string clientPort;

    switch(this->actionType_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE: {
            auto dataDialog = dynamic_pointer_cast<CompanionDataDialog>(this->dialog_);

            if (dataDialog) {
                name = dataDialog->getNameString();
                ipAddress = dataDialog->getIpAddressString();
                clientPort = dataDialog->getPortString();
            }
        }

        break;

    case ChatActionType::DELETE:
    case ChatActionType::CLEAR_HISTORY: {
            name = this->companion_->getName();
            ipAddress = this->companion_->getSocketIpAddress();
            clientPort = std::to_string(this->companion_->getSocketClientPort());
        }

        break;
    }

    logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    this->data_ = std::make_shared<CompanionData>(name, ipAddress, serverPort, clientPort);

    // TODO change
    auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

    getGraphicManager()->sendCompanionDataToManager(cast);
}

GroupChatAction::GroupChatAction(ChatActionType actionType)
    : actionType_(actionType), data_(new GroupChatData), Action(nullptr) {

    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch(actionType) {
    case ChatActionType::CREATE:
        dialog_ = std::make_shared<GroupChatDataDialog>(actionType_, mainWindow);

        break;
    }
}

void GroupChatAction::sendData() {

}

PasswordAction::PasswordAction(PasswordActionType actionType) : Action(nullptr) {
    actionType_ = actionType;

    switch(actionType) {
    case PasswordActionType::CREATE:
        dialog_ = std::make_shared<CreatePasswordDialog>();

        break;

    case PasswordActionType::GET:
        dialog_ = std::make_shared<GetPasswordDialog>();

        break;
    }
}

PasswordAction::~PasswordAction() {
    if(this->actionType_ == PasswordActionType::GET) {
        this->dialog_->close();
    }
}

std::string PasswordAction::getPassword()
{
    return this->password_;
}

void PasswordAction::sendData() {
    switch(this->actionType_) {
    case PasswordActionType::CREATE: {
            auto passwordDialog = dynamic_pointer_cast<CreatePasswordDialog>(this->dialog_);

            auto text1 = passwordDialog->getFirstEditText();
            auto text2 = passwordDialog->getSecondEditText();

            if(text1 == text2) {
                if(text1.size() == 0) {
                    showErrorDialogAndLogError(
                        "Empty password is invalid", this->getDialog());

                    return;
                }

                this->password_ = text1;

                // TODO change
                auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

                getGraphicManager()->sendNewPasswordDataToManager(cast);
            }
            else {
                showErrorDialogAndLogError(
                    "Entered passwords are not equal", this->getDialog());
            }
        }

        break;

    case PasswordActionType::GET: {
            auto passwordDialog = dynamic_pointer_cast<GetPasswordDialog>(this->dialog_);

            auto text = passwordDialog->getEditText();

            if(text.size() == 0) {
                showErrorDialogAndLogError(
                    "Empty password is invalid", this->getDialog());

                return;
            }

            this->password_ = text;

            // TODO change
            auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

            getGraphicManager()->sendExistingPasswordDataToManager(cast);
        }

        break;
    }
}

FileAction::FileAction(
    FileActionType actionType, const std::string& networkId, std::shared_ptr<Companion> companion)
    : Action(nullptr)
{
    actionType_ = actionType;
    companion_ = companion;
    networkId_ = networkId;

    QString windowTitle = getConstantMappingValue(
        "fileDialogTypeQStringRepresentation",
        &fileDialogTypeQStringRepresentation,
        actionType);

    // TODO change
    auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

    dialog_ = std::make_shared<FileDialog>(cast, windowTitle);
}

FileActionType FileAction::getType() const {
    return this->actionType_;
}

std::shared_ptr<Companion> FileAction::getCompanion() const {
    return this->companion_;
}

std::filesystem::path FileAction::getPath() const {
    return this->filePath_;
}

void FileAction::sendData() {
    logArgs("FileAction::sendData");

    auto cast = dynamic_pointer_cast<FileDialog>(this->dialog_);

    if (!cast)
        return;

    auto dialog = cast->getFileDialog();

    if (!dialog)
        return;

    switch(this->actionType_) {
    case FileActionType::SEND: {
            for(auto& pathQString : dialog->selectedFiles()) {  // one file
                logArgs(pathQString);

                auto path = std::filesystem::path(pathQString.toStdString());

                this->filePath_ = path;  // TODO ???

                getManager()->sendMessage(
                    MessageType::FILE, this->getCompanion(), shared_from_this(),
                    std::format("SEND FILE: {}", this->filePath_.filename().string()));

                getManager()->setLastOpenedPath(path.parent_path());
            }
        }

        break;

    case FileActionType::SAVE: {
            // for(auto& pathQString : dialog->selectedFiles())  // one file
            // {
            //     logArgs(pathQString);

            //     auto path = std::filesystem::path(pathQString.toStdString());

            //     this->filePath_ = path;

            //     // set file path for file operator
            //     this->companion_->getFileOperatorStorage()->
            //         getOperator(this->networkId_)->setFilePath(path);

            //     // send without saving to db
            //     bool result = this->companion_->sendMessage(
            //         false, NetworkMessageType::FILE_REQUEST,
            //         this->networkId_, nullptr);

            //     getManager()->setLastOpenedPath(path.parent_path());

            // }

            // set file path for file operator
            bool setResult = this->companion_->setFileOperatorFilePath(
                this->networkId_, this->filePath_);

            if(setResult) {
                // send without saving to db
                bool result = this->companion_->sendMessage(
                    false, NetworkMessageType::FILE_REQUEST,
                    this->networkId_, nullptr);

                getManager()->setLastOpenedPath(this->filePath_.parent_path());
            }
            else {
                logTemplateError("error saving file, path: {}", this->filePath_.string());
            }
        }

        break;
    }
}

void FileAction::defineFilePath() {
    this->filePath_ = QFileDialog::getSaveFileName(
        dynamic_pointer_cast<QWidget>(getGraphicManager()->getMainWindow()).get(),
        "Save File",
        // getQString(getManager()->getLastOpenedPath().string())).toStdString();
        getQString(getManager()->getLastOpenedPath().string())).toStdString();

    this->sendData();
}
