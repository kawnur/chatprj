#include "action.hpp"

#include <format>

#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

Action::Action(std::shared_ptr<Dialog> dialog) : dialog_(dialog) {}

std::shared_ptr<Dialog> Action::getDialog()
{
    return dialog_;
}

void Action::set()
{
    dialog_->setAction(shared_from_this());
    dialog_->set();

    // if (dialog_->getContainsDialog())

    auto actionCast = dynamic_pointer_cast<FileAction>(shared_from_this());

    if (!actionCast) {
        dialog_->show();

        return;
    }

    switch (actionCast->getType()) {
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

CompanionAction::CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion)
    : type_(type), companion_(companion), data_(nullptr), Action(nullptr)
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
        dialog_ = std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);

    break;

    case ChatActionType::DELETE:
        dialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, deleteCompanionDialogText,
            getButtonInfoVector(deleteCompanionButtonText));

    break;

    case ChatActionType::CLEAR_HISTORY:
        dialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, clearCompanionHistoryDialogText,
            getButtonInfoVector(clearHistoryButtonText));

    break;

    case ChatActionType::SEND_HISTORY:
        auto name = companion->getName();

        dialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING,
            std::vformat(sendChatHistoryToCompanionDialogText, std::make_format_args(name)),
            getButtonInfoVector(sendChatHistoryButtonText));

    break;
    }
}

ChatActionType CompanionAction::getType() const
{
    return type_;
}

std::string CompanionAction::getName() const
{
    return data_->getName();
}

std::string CompanionAction::getIpAddress() const
{
    return data_->getIpAddress();
}

std::string CompanionAction::getServerPort() const
{
    return data_->getServerPort();
}

std::string CompanionAction::getClientPort() const
{
    return data_->getClientPort();
}

int CompanionAction::getCompanionId() const
{
    return companion_->getId();
}

std::shared_ptr<Companion> CompanionAction::getCompanion() const
{
    return companion_;
}

void CompanionAction::updateCompanionObjectData()
{
    companion_->updateData(data_);
}

// TODO deletion of action objects
void CompanionAction::sendData()
{
    if (type_ == ChatActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(companion_);

        return;
    }

    std::string name;
    std::string ipAddress;
    std::string serverPort { "" };
    std::string clientPort;

    switch (type_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
    {
        auto dataDialog = dynamic_pointer_cast<CompanionDataDialog>(dialog_);

        if (dataDialog) {
            name = dataDialog->getNameString();
            ipAddress = dataDialog->getIpAddressString();
            clientPort = dataDialog->getPortString();
        }
    }

    break;

    case ChatActionType::DELETE:
    case ChatActionType::CLEAR_HISTORY:
    {
        name = companion_->getName();
        ipAddress = companion_->getSocketIpAddress();
        clientPort = std::to_string(companion_->getSocketClientPort());
    }

    break;
    }

    logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    data_ = std::make_shared<CompanionData>(name, ipAddress, serverPort, clientPort);

    // TODO change
    auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

    getGraphicManager()->sendCompanionDataToManager(cast);
}

GroupChatAction::GroupChatAction(ChatActionType type)
    : type_(type), data_(new GroupChatData), Action(nullptr)
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type) {
    case ChatActionType::CREATE:
        dialog_ = std::make_shared<GroupChatDataDialog>(type_, mainWindow);

        break;
    }
}

PasswordAction::PasswordAction(PasswordActionType type) : Action(nullptr)
{
    type_ = type;

    switch (type) {
    case PasswordActionType::CREATE:
        dialog_ = std::make_shared<CreatePasswordDialog>();

        break;

    case PasswordActionType::GET:
        dialog_ = std::make_shared<GetPasswordDialog>();

        break;
    }
}

PasswordAction::~PasswordAction()
{
    if (type_ == PasswordActionType::GET)
        dialog_->close();
}

std::string PasswordAction::getPassword()
{
    return password_;
}

void PasswordAction::sendData()
{
    switch (type_) {
    case PasswordActionType::CREATE:
    {
        auto passwordDialog = dynamic_pointer_cast<CreatePasswordDialog>(dialog_);

        if (!passwordDialog)
            break;

        auto text1 = passwordDialog->getFirstEditText();
        auto text2 = passwordDialog->getSecondEditText();

        if (text1 == text2) {
            if (text1.size() == 0) {
                showErrorDialogAndLogError("Empty password is invalid", getDialog());

                return;
            }

            password_ = text1;

            // TODO change
            auto cast =
                dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

            getGraphicManager()->sendNewPasswordDataToManager(cast);
        }
        else {
            showErrorDialogAndLogError("Entered passwords are not equal", getDialog());
        }
    }

    break;

    case PasswordActionType::GET:
    {
        auto passwordDialog = dynamic_pointer_cast<GetPasswordDialog>(dialog_);

        if (!passwordDialog)
            break;

        auto text = passwordDialog->getEditText();

        if (text.size() == 0) {
            showErrorDialogAndLogError("Empty password is invalid", getDialog());

            return;
        }

        password_ = text;

        // TODO change
        auto cast =
            dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

        getGraphicManager()->sendExistingPasswordDataToManager(cast);
    }

    break;
    }
}

FileAction::FileAction(
    FileActionType type, const std::string &networkId, std::shared_ptr<Companion> companion)
    : Action(nullptr)
{
    type_ = type;
    companion_ = companion;
    networkId_ = networkId;

    auto windowTitle = getConstantMappingValue(
        "fileDialogTypeQStringRepresentation",
        &fileDialogTypeQStringRepresentation,
        type);

    // TODO change
    auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

    dialog_ = std::make_shared<FileDialog>(cast, windowTitle);
}

FileActionType FileAction::getType() const
{
    return type_;
}

std::shared_ptr<Companion> FileAction::getCompanion() const
{
    return companion_;
}

std::filesystem::path FileAction::getPath() const
{
    return filePath_;
}

void FileAction::sendData()
{
    logArgs("FileAction::sendData");

    auto cast = dynamic_pointer_cast<FileDialog>(dialog_);

    if (!cast)
        return;

    auto dialog = cast->getFileDialog();

    if (!dialog)
        return;

    switch (type_) {
    case FileActionType::SEND:
    {
        for (auto &pathQString : dialog->selectedFiles()) {  // one file
            logArgs(pathQString);

            auto path = std::filesystem::path(pathQString.toStdString());

            filePath_ = path;  // TODO ???

            getManager()->sendMessage(
                MessageType::FILE, getCompanion(), shared_from_this(),
                std::format("SEND FILE: {}", filePath_.filename().string()));

            getManager()->setLastOpenedPath(path.parent_path());
        }
    }

    break;

    case FileActionType::SAVE:
    {
        // for (auto &pathQString : dialog->selectedFiles())  // one file
        // {
        //     logArgs(pathQString);

        //     auto path = std::filesystem::path(pathQString.toStdString());

        //     filePath_ = path;

        //     // set file path for file operator
        //     companion_->getFileOperatorStorage()->
        //         getOperator(networkId_)->setFilePath(path);

        //     // send without saving to db
        //     bool result = companion_->sendMessage(
        //         false, NetworkMessageType::FILE_REQUEST,
        //         networkId_, nullptr);

        //     getManager()->setLastOpenedPath(path.parent_path());

        // }

        // set file path for file operator
        bool setResult = companion_->setFileOperatorFilePath(networkId_, filePath_);

        if (setResult) {
            // send without saving to db
            bool result = companion_->sendMessage(
                false, NetworkMessageType::FILE_REQUEST, networkId_, nullptr);

            getManager()->setLastOpenedPath(filePath_.parent_path());
        }
        else {
            logTemplateError("error saving file, path: {}", filePath_.string());
        }
    }

    break;
    }
}

void FileAction::defineFilePath()
{
    auto cast = dynamic_pointer_cast<QWidget>(getGraphicManager()->getMainWindow());

    if (!cast)
        return;

    auto pathQStr = QString::fromStdString(getManager()->getLastOpenedPath().string());

    QString param { "Save File" };
    auto result = QFileDialog::getSaveFileName(cast.get(), param, pathQStr);
    filePath_ = std::filesystem::path(result.toStdString());
    sendData();
}
