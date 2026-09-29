#include "action.hpp"

#include <string>

#include "action_wrapper.hpp"
#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

using namespace std::string_literals;

// std::shared_ptr<Dialog> ActionWrapper::getDialog()
// {
//     return dialog_;
// }

// void ActionWrapper::set(std::shared_ptr<Action> action)
// {
//     action_ = action;
//     dialog_->setWrapper(shared_from_this());
//     dialog_->set();
// }

// void ActionWrapper::showDialog()
// {
//     dialog_->show();
// }

// void ActionWrapper::act()
// {
//     action_->act();
// }

// Action::Action(std::shared_ptr<Dialog> dialog) : dialog_(dialog) {}
Action::Action() {}

Action::~Action() { logArgsInfo(Q_FUNC_INFO); }

// std::shared_ptr<Dialog> Action::getDialog()
// {
//     return dialog_;
// }

// void Action::setDialog(std::shared_ptr<Action> action)
// void Action::setWrapper(std::shared_ptr<Action> action, std::shared_ptr<Dialog> dialog)
void Action::setWrapper(std::shared_ptr<ActionWrapperBase> wrapper)
{
    // wrapper_ = std::make_shared<DialogWrapper>(dialog);
    // wrapper_->set(action);
    // logArgsInfo(Q_FUNC_INFO, "action.use_count():", action.use_count());

    // dialog_->setAction(action);
    // dialog_->set();
    // logArgsInfo(Q_FUNC_INFO, "action.use_count():", action.use_count());
    wrapper_ = wrapper;
}

void Action::failed()
{
    logArgsInfo(Q_FUNC_INFO);
    // dialog_->close();
}

// RegularAction::RegularAction(std::shared_ptr<Dialog> dialog) : Action(dialog) {}
RegularAction::RegularAction(std::shared_ptr<Dialog> dialog) : Action() {}

RegularAction::~RegularAction() { logArgsInfo(Q_FUNC_INFO); }

void RegularAction::set()
{
    // setDialog();
    // dialog_->show();
    // wrapper_->showDialog();
}

CompanionAction::CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion)
    : type_(type), companion_(companion), data_(nullptr), RegularAction(nullptr)
{}
//     std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();
//     // std::shared_ptr<Dialog> dialog = nullptr;

//     switch (type) {
//     case ChatActionType::CREATE:
//     case ChatActionType::UPDATE:
//         dialog_ = std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);
//         // dialog = std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);

//     break;

//     case ChatActionType::DELETE:
//         dialog_ = std::make_shared<TextDialog>(
//         // dialog = std::make_shared<TextDialog>(
//             mainWindow, DialogType::WARNING, deleteCompanionDialogText,
//             getButtonInfoVector(deleteCompanionButtonText));

//     break;

//     case ChatActionType::CLEAR_HISTORY:
//         dialog_ = std::make_shared<TextDialog>(
//         // dialog = std::make_shared<TextDialog>(
//             mainWindow, DialogType::WARNING, clearCompanionHistoryDialogText,
//             getButtonInfoVector(clearHistoryButtonText));

//     break;

//     case ChatActionType::SEND_HISTORY:
//         auto name = companion->getName();

//         dialog_ = std::make_shared<TextDialog>(
//         // dialog = std::make_shared<TextDialog>(
//             mainWindow, DialogType::WARNING,
//             getStringByFormat(sendChatHistoryToCompanionDialogText, name),
//             getButtonInfoVector(sendChatHistoryButtonText));

//     break;
//     }
// }

CompanionAction::~CompanionAction() { logArgsInfo(Q_FUNC_INFO); }

// ChatActionType CompanionAction::getType() const
ChatActionType CompanionAction::getType()
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

std::shared_ptr<Dialog> CompanionAction::buildDialog()
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
        return std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);

    case ChatActionType::DELETE:
        return std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, deleteCompanionDialogText,
            getButtonInfoVector(deleteCompanionButtonText));

    case ChatActionType::CLEAR_HISTORY:
        return std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, clearCompanionHistoryDialogText,
            getButtonInfoVector(clearHistoryButtonText));

    case ChatActionType::SEND_HISTORY:
        auto name = companion_->getName();

        return std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING,
            getStringByFormat(sendChatHistoryToCompanionDialogText, name),
            getButtonInfoVector(sendChatHistoryButtonText));

        break;
    }
}

void CompanionAction::set()
{
    // setWrapper(std::dynamic_pointer_cast<Action>(shared_from_this()), dialog);

    // auto cast = dynamic_pointer_cast<Action>(shared_from_this());

    // if (!cast)
    //     logArgsError("action cast error");

    // setDialog(cast);
    // setDialog(shared_from_this());

    // dialog_->show();
}

void CompanionAction::updateCompanionObjectData()
{
    companion_->updateData(data_);
}

// TODO deletion of action objects
void CompanionAction::act()
{
    if (type_ == ChatActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(companion_);

        return;
    }

    // std::string name;
    // std::string ipAddress;
    // std::string serverPort { "" };
    // std::string clientPort;

    switch (type_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
    {
        // auto dataDialog = dynamic_pointer_cast<CompanionDataDialog>(dialog_);

        // if (dataDialog) {
        //     name = dataDialog->getNameString();
        //     ipAddress = dataDialog->getIpAddressString();
        //     clientPort = dataDialog->getPortString();
        // }

        // auto dialog = wrapper_->getDialog();
        // auto data = dialog_->getCompanionData();
        auto data = wrapper_->getCompanionData();

        if (!data)
            return;

        data_ = data;
    }

    break;

    case ChatActionType::DELETE:
    case ChatActionType::CLEAR_HISTORY:
    {
        // name = companion_->getName();
        // ipAddress = companion_->getSocketIpAddress();
        // clientPort = std::to_string(companion_->getSocketClientPort());
        data_ = std::make_shared<CompanionData>(
            companion_->getName(), companion_->getSocketIpAddress(), ""s,
            std::to_string(companion_->getSocketClientPort()));
    }

    break;
    }

    data_->log();
    // logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    // data_ = std::make_shared<CompanionData>(name, ipAddress, serverPort, clientPort);

    // TODO change
    // auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

    // getGraphicManager()->sendCompanionDataToManager(cast);

    // auto cast = std::dynamic_pointer_cast<CompanionAction>(shared_from_this());

    // if (!cast) {
    //     logArgsError("action cast error");

    //     return;
    // }

    // getGraphicManager()->sendCompanionDataToManager(shared_from_this());
    // getManager()->performCompanionAction(shared_from_this());
    // getManager()->performCompanionAction(wrapper_);
}

// void CompanionAction::failed()
// {
//     dialog_->close();
// }

void CompanionAction::method1()
{
    logArgsInfo(Q_FUNC_INFO, "wrapper.use_count():", wrapper_.use_count());

    wrapper_.reset();

    logArgsInfo(Q_FUNC_INFO, "wrapper.use_count():", wrapper_.use_count());
}

// GroupChatAction::GroupChatAction(ChatActionType type)
//     : type_(type), data_(new GroupChatData), RegularAction(nullptr)
// {
//     std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

//     switch (type) {
//     case ChatActionType::CREATE:
//         dialog_ = std::make_shared<GroupChatDataDialog>(type_, mainWindow);

//         break;
//     }
// }

// PasswordAction::PasswordAction(PasswordActionType type) : RegularAction(nullptr)
// {
//     type_ = type;

//     switch (type) {
//     case PasswordActionType::CREATE:
//         dialog_ = std::make_shared<CreatePasswordDialog>();

//         break;

//     case PasswordActionType::GET:
//         dialog_ = std::make_shared<GetPasswordDialog>();

//         break;
//     }
// }

// PasswordAction::~PasswordAction()
// {
//     if (type_ == PasswordActionType::GET)
//         dialog_->close();
// }

// std::string PasswordAction::getPassword()
// {
//     return password_;
// }

// void PasswordAction::act()
// {
//     switch (type_) {
//     case PasswordActionType::CREATE:
//     {
//         auto passwordDialog = dynamic_pointer_cast<CreatePasswordDialog>(dialog_);

//         if (!passwordDialog)
//             break;

//         auto text1 = passwordDialog->getFirstEditText();
//         auto text2 = passwordDialog->getSecondEditText();

//         if (text1 == text2) {
//             if (text1.size() == 0) {
//                 showErrorDialogAndLogError("Empty password is invalid", getDialog());

//                 return;
//             }

//             password_ = text1;

//             // TODO change
//             // auto cast =
//             //     dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

//             // getGraphicManager()->sendNewPasswordDataToManager(cast);
//             getGraphicManager()->sendNewPasswordDataToManager(shared_from_this());
//         }
//         else {
//             showErrorDialogAndLogError("Entered passwords are not equal", getDialog());
//         }
//     }

//     break;

//     case PasswordActionType::GET:
//     {
//         auto passwordDialog = dynamic_pointer_cast<GetPasswordDialog>(dialog_);

//         if (!passwordDialog)
//             break;

//         auto text = passwordDialog->getEditText();

//         if (text.size() == 0) {
//             showErrorDialogAndLogError("Empty password is invalid", getDialog());

//             return;
//         }

//         password_ = text;

//         // TODO change
//         auto cast =
//             dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

//         getGraphicManager()->sendExistingPasswordDataToManager(cast);
//     }

//     break;
//     }
// }

// FileAction::FileAction(
//     FileActionType type, const std::string &networkId, std::shared_ptr<Companion> companion)
//     : Action()
// {
//     type_ = type;
//     companion_ = companion;
//     networkId_ = networkId;

//     auto windowTitle = getMapValue(fileDialogTypeQStringRepresentation, type, "File action"s);

//     // TODO change
//     // auto cast = dynamic_pointer_cast<std::remove_reference_t<decltype(*this)>>(shared_from_this());

//     // dialog_ = std::make_shared<FileDialog>(cast, windowTitle);
//     dialog_ = std::make_shared<FileDialog>(shared_from_this(), windowTitle);
// }

// std::shared_ptr<Companion> FileAction::getCompanion() const
// {
//     return companion_;
// }

// std::filesystem::path FileAction::getPath() const
// {
//     return filePath_;
// }

// void FileAction::set()
// {
//     // setDialog();
//     setDialog(std::dynamic_pointer_cast<Action>(shared_from_this()));

//     switch (type_) {
//     case FileActionType::SEND:
//         dialog_->showDialog();
//         break;
//     case FileActionType::SAVE:
//         defineFilePath();
//         break;
//     default:
//         logArgsError("unknown action type");
//         break;
//     }
// }

// void FileAction::act()
// {
//     logArgs(__FUNCTION__);

//     auto cast = dynamic_pointer_cast<FileDialog>(dialog_);

//     if (!cast)
//         return;

//     auto dialog = cast->getFileDialog();

//     if (!dialog)
//         return;

//     switch (type_) {
//     case FileActionType::SEND:
//     {
//         for (const auto &pathQString : dialog->selectedFiles()) {  // one file
//             logArgs(pathQString);

//             auto path = std::filesystem::path(pathQString.toStdString());

//             filePath_ = path;  // TODO ???

//             getManager()->sendMessage(
//                 MessageType::FILE, getCompanion(), shared_from_this(),
//                 getStringByFormat("SEND FILE: {}", filePath_.filename().string()));

//             getManager()->setLastOpenedPath(path.parent_path());
//         }
//     }

//     break;

//     case FileActionType::SAVE:
//     {
//         // for (auto &pathQString : dialog->selectedFiles())  // one file
//         // {
//         //     logArgs(pathQString);

//         //     auto path = std::filesystem::path(pathQString.toStdString());

//         //     filePath_ = path;

//         //     // set file path for file operator
//         //     companion_->getFileOperatorStorage()->
//         //         getOperator(networkId_)->setFilePath(path);

//         //     // send without saving to db
//         //     bool result = companion_->sendMessage(
//         //         false, NetworkMessageType::FILE_REQUEST,
//         //         networkId_, nullptr);

//         //     getManager()->setLastOpenedPath(path.parent_path());

//         // }

//         // set file path for file operator
//         bool setResult = companion_->setFileOperatorFilePath(networkId_, filePath_);

//         if (setResult) {
//             // send without saving to db
//             auto meta = std::make_shared<MessageMetaData>();
//             meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;
//             meta->networkId_ = networkId_;

//             bool result = companion_->sendMessage(nullptr, meta);

//             getManager()->setLastOpenedPath(filePath_.parent_path());
//         }
//         else {
//             logTemplateError("error saving file, path: {}", filePath_.string());
//         }
//     }

//     break;
//     }
// }

// void FileAction::defineFilePath()
// {
//     auto cast = dynamic_pointer_cast<QWidget>(getGraphicManager()->getMainWindow());

//     if (!cast)
//         return;

//     auto pathQStr = QString::fromStdString(getManager()->getLastOpenedPath().string());

//     QString param { "Save File" };
//     auto result = QFileDialog::getSaveFileName(cast.get(), param, pathQStr);
//     filePath_ = std::filesystem::path(result.toStdString());
//     act();
// }
