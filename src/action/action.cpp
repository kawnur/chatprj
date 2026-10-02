#include "action.hpp"

#include <string>

#include "action_result.hpp"
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

Action::Action() {}

Action::~Action() { logArgsInfo(Q_FUNC_INFO); }

void Action::setDialog(std::shared_ptr<Action> action)
{
    dataDialog_->set(action);
    infoDialog_->set(action);
}

RegularAction::RegularAction() : Action() {}

RegularAction::~RegularAction() { logArgsInfo(Q_FUNC_INFO); }

void RegularAction::set()
{
    // setDialog();
    dataDialog_->show();
}

CompanionAction::CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion)
    : type_(type), companion_(companion), data_(nullptr), RegularAction()
{
    buildDataDialog();
    buildInfoDialog();
}

CompanionAction::~CompanionAction() { logArgsInfo(Q_FUNC_INFO); }

void CompanionAction::buildDataDialog()
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE:
        dataDialog_ = std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);

        break;

    case ChatActionType::DELETE:
        dataDialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, deleteCompanionDialogText,
            getButtonInfoVector(deleteCompanionButtonText));

        break;

    case ChatActionType::CLEAR_HISTORY:
        dataDialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING, clearCompanionHistoryDialogText,
            getButtonInfoVector(clearHistoryButtonText));

        break;

    case ChatActionType::SEND_HISTORY: {
        auto name = companion_->getName();

        dataDialog_ = std::make_shared<TextDialog>(
            mainWindow, DialogType::WARNING,
            getStringByFormat(sendChatHistoryToCompanionDialogText, name),
            getButtonInfoVector(sendChatHistoryButtonText));
    }

    break;

    default:
        break;
    }
}

void CompanionAction::buildInfoDialog()
{
    auto function = [](TextDialog &dialog) { dialog.closeSelfAndParentDialog(); };

    infoDialog_ = std::make_shared<TextDialog>(
        dataDialog_, DialogType::INFO, ""s, createOkButtonInfoVector(function));
}

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

std::shared_ptr<CompanionData> CompanionAction::getCompanionData() const
{
    return data_;
}

std::shared_ptr<Companion> CompanionAction::getCompanion() const
{
    return companion_;
}

void CompanionAction::set()
{
    auto cast = dynamic_pointer_cast<Action>(shared_from_this());

    if (!cast)
        logArgsError("action cast error");

    setDialog(cast);

    dataDialog_->show();
}

void CompanionAction::updateCompanionObjectData()
{
    companion_->updateData(data_);
}

std::string CompanionAction::getInfoDialogHeader(const auto &map)
{
    return getMapValue(map, type_, COMPANION_ACTION_INFO_DIALOG_DEFAULT_HEADER);
}

std::string CompanionAction::getInfoDialogSuccessHeader()
{
    return getInfoDialogHeader(COMPANION_ACTION_INFO_DIALOG_SUCCESS_HEADER_MAP);
}

std::string CompanionAction::getInfoDialogFailHeader()
{
    return getInfoDialogHeader(COMPANION_ACTION_INFO_DIALOG_FAIL_HEADER_MAP);
}

void CompanionAction::updateInfoDialog(std::shared_ptr<ActionResult> result)
{
    std::string text {};
    std::string header {};
    auto messages = std::vector<std::string> {};

    if (result->status()) {
        std::vector<std::pair<std::string, std::string>> lines {
            { "name: {}", data_->getName() },
            { "ipAddress: {}", data_->getIpAddress() },
            { "port: {}", data_->getClientPort() }
        };

        header = getInfoDialogSuccessHeader();
        fillMessages(messages, lines);
    } else {
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
    }

    text = buildTextAsUnorderedListWithHeader(header, messages);
    infoDialog_->setText(text);
}

void CompanionAction::act()
{
    if (type_ == ChatActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(companion_);

        return;
    }

    switch (type_) {
    case ChatActionType::CREATE:
    case ChatActionType::UPDATE: {
        auto cast = dynamic_pointer_cast<CompanionDataDialog>(dataDialog_);

        if (!cast)
            break;

        data_ = std::make_shared<CompanionData>(
            cast->getNameString(), cast->getIpAddressString(), ""s, cast->getPortString());
    }

    break;

    case ChatActionType::DELETE:
    case ChatActionType::CLEAR_HISTORY:
        data_ = std::make_shared<CompanionData>(
            companion_->getName(), companion_->getSocketIpAddress(), ""s,
            std::to_string(companion_->getSocketClientPort()));

    break;

    default:
        break;
    }

    data_->log();

    getManager()->performCompanionAction(shared_from_this());
}

void CompanionAction::postAct(std::shared_ptr<ActionResult> result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

void CompanionAction::endAct()
{
    getManager()->endAction(shared_from_this());
}

GroupChatAction::GroupChatAction(ChatActionType type)
    : type_(type), data_(new GroupChatData), RegularAction()
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type) {
    case ChatActionType::CREATE:
        dataDialog_ = std::make_shared<GroupChatDataDialog>(type_, mainWindow);

        break;
    }
}

PasswordAction::PasswordAction(PasswordActionType type) : RegularAction()
{
    type_ = type;

    switch (type) {
    case PasswordActionType::CREATE:
        dataDialog_ = std::make_shared<CreatePasswordDialog>();

        break;

    case PasswordActionType::GET:
        dataDialog_ = std::make_shared<GetPasswordDialog>();

        break;
    }
}

PasswordAction::~PasswordAction()
{
    if (type_ == PasswordActionType::GET)
        dataDialog_->close();
}

std::string PasswordAction::getPassword()
{
    return password_;
}

void PasswordAction::act()
{
    switch (type_) {
    case PasswordActionType::CREATE:
    {
        auto passwordDialog = dynamic_pointer_cast<CreatePasswordDialog>(dataDialog_);

        if (!passwordDialog)
            break;

        auto text1 = passwordDialog->getFirstEditText();
        auto text2 = passwordDialog->getSecondEditText();

        if (text1 == text2) {
            if (text1.size() == 0) {
                // showErrorDialogAndLogError("Empty password is invalid", getDialog());
                showErrorDialogAndLogError("Empty password is invalid", dataDialog_);

                return;
            }

            password_ = text1;

            getGraphicManager()->sendNewPasswordDataToManager(shared_from_this());
        }
        else {
            // showErrorDialogAndLogError("Entered passwords are not equal", getDialog());
            showErrorDialogAndLogError("Entered passwords are not equal", dataDialog_);
        }
    }

    break;

    case PasswordActionType::GET:
    {
        auto passwordDialog = dynamic_pointer_cast<GetPasswordDialog>(dataDialog_);

        if (!passwordDialog)
            break;

        auto text = passwordDialog->getEditText();

        if (text.size() == 0) {
            // showErrorDialogAndLogError("Empty password is invalid", getDialog());
            showErrorDialogAndLogError("Empty password is invalid", dataDialog_);

            return;
        }

        password_ = text;

        getGraphicManager()->sendExistingPasswordDataToManager(shared_from_this());
    }

    break;
    }
}

FileAction::FileAction(
    FileActionType type, const std::string &networkId, std::shared_ptr<Companion> companion)
    : Action()
{
    type_ = type;
    companion_ = companion;
    networkId_ = networkId;

    auto windowTitle = getMapValue(fileDialogTypeQStringRepresentation, type, "File action"s);
    dataDialog_ = std::make_shared<FileDialog>(shared_from_this(), windowTitle);
}

std::shared_ptr<Companion> FileAction::getCompanion() const
{
    return companion_;
}

std::filesystem::path FileAction::getPath() const
{
    return filePath_;
}

void FileAction::set()
{
    setDialog(std::dynamic_pointer_cast<Action>(shared_from_this()));

    switch (type_) {
    case FileActionType::SEND:
        dataDialog_->showDialog();
        break;
    case FileActionType::SAVE:
        defineFilePath();
        break;
    default:
        logArgsError("unknown action type");
        break;
    }
}

void FileAction::act()
{
    logArgs(__FUNCTION__);

    auto cast = dynamic_pointer_cast<FileDialog>(dataDialog_);

    if (!cast)
        return;

    auto dialog = cast->getFileDialog();

    if (!dialog)
        return;

    switch (type_) {
    case FileActionType::SEND:
    {
        for (const auto &pathQString : dialog->selectedFiles()) {  // one file
            logArgs(pathQString);

            auto path = std::filesystem::path(pathQString.toStdString());

            filePath_ = path;  // TODO ???

            getManager()->sendMessage(
                MessageType::FILE, getCompanion(), shared_from_this(),
                getStringByFormat("SEND FILE: {}", filePath_.filename().string()));

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
            auto meta = std::make_shared<MessageMetaData>();
            meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;
            meta->networkId_ = networkId_;

            bool result = companion_->sendMessage(nullptr, meta);

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
    act();
}

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines)
{
    for (const auto &line : lines)
        messages.emplace_back(getStringByFormat(line.first, line.second));
}
