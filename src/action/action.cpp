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
// #include "utils_widgets.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

using namespace std::string_literals;

Action::Action(ActionType type) : type_(type) {}

Action::~Action() { logArgsInfo(Q_FUNC_INFO); }

void Action::setDialog(std::shared_ptr<Action> action)
{
    dataDialog_->set(action);
    infoDialog_->set(action);
}

ActionType Action::getType()
{
    return type_;
}

std::string Action::getInfoDialogHeader(const auto &map)
{
    return getMapValue(map, type_, ACTION_INFO_DIALOG_DEFAULT_HEADER);
}

std::string Action::getInfoDialogSuccessHeader()
{
    return getInfoDialogHeader(ACTION_SUCCESS_HEADER_MAP);
}

std::string Action::getInfoDialogFailHeader()
{
    return getInfoDialogHeader(ACTION_FAIL_HEADER_MAP);
}

RegularAction::RegularAction(ActionType type) : Action(type) {}

RegularAction::~RegularAction() { logArgsInfo(Q_FUNC_INFO); }

void RegularAction::set()
{
    // setDialog();
    dataDialog_->show();
}

CompanionAction::CompanionAction(ActionType type, std::shared_ptr<Companion> companion)
    : companion_(companion), data_(nullptr), RegularAction(type) {}

CompanionAction::~CompanionAction() { logArgsInfo(Q_FUNC_INFO); }

void CompanionAction::buildDataDialog()
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type_) {
    case ActionType::CREATE_COMPANION:
    case ActionType::UPDATE_COMPANION:
        dataDialog_ = std::make_shared<CompanionDataDialog>(type_, mainWindow, companion_);

        break;

    case ActionType::DELETE_COMPANION: {
        std::initializer_list<ButtonInfo> list {
            { ButtonType::DELETE_COMPANION, [](TextDialog &dialog) { dialog.acceptAction(); } }
        };

        dataDialog_ = std::make_shared<TextDialog>(
            nullptr, DialogType::WARNING, deleteCompanionDialogText,
            // getButtonInfoVector(ButtonType::DELETE_COMPANION));
            list);
    }

    break;

    case ActionType::CLEAR_HISTORY:
        // dataDialog_ = std::make_shared<TextDialog>(
        //     mainWindow, DialogType::WARNING, clearCompanionHistoryDialogText,
        //     getButtonInfoVector(ButtonType::CLEAR_HISTORY));

        break;

    case ActionType::SEND_HISTORY: {
        // auto name = companion_->getName();

        // dataDialog_ = std::make_shared<TextDialog>(
        //     mainWindow, DialogType::WARNING,
        //     getStringByFormat(sendChatHistoryToCompanionDialogText, name),
        //     getButtonInfoVector(ButtonType::SEND_HISTORY));
    }

    break;

    default:
        break;
    }
}

void CompanionAction::buildInfoDialog()
{
    auto function = [](TextDialog &dialog) { dialog.closeSelfAndParentDialog(); };

    std::initializer_list<ButtonInfo> list { { ButtonType::OK, function } };

    infoDialog_ = std::make_shared<TextDialog>(
        // dataDialog_, DialogType::INFO, ""s, createOkButtonInfoVector(function));
        dataDialog_, DialogType::INFO, ""s, list);
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

// TODO change
void CompanionAction::set()
{
    buildDataDialog();
    buildInfoDialog();

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
        infoDialog_->setDialogType(DialogType::ERROR);
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
    }

    text = buildTextAsUnorderedListWithHeader(header, messages);
    infoDialog_->setText(text);
}

void CompanionAction::act()
{
    // TODO move inside switch
    if (type_ == ActionType::SEND_HISTORY) {
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(companion_);

        return;
    }

    switch (type_) {
    case ActionType::CREATE_COMPANION:
    case ActionType::UPDATE_COMPANION: {
        auto cast = dynamic_pointer_cast<CompanionDataDialog>(dataDialog_);

        if (!cast)
            break;

        data_ = std::make_shared<CompanionData>(
            cast->getNameString(), cast->getIpAddressString(), ""s, cast->getPortString());
    }

    break;

    case ActionType::DELETE_COMPANION:
    case ActionType::CLEAR_HISTORY:
        data_ = std::make_shared<CompanionData>(
            companion_->getName(), companion_->getSocketIpAddress(), ""s,
            std::to_string(companion_->getSocketClientPort()));

    break;

    default:
        break;
    }

    data_->log();

    getManager()->performAction(shared_from_this());
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

GroupChatAction::GroupChatAction(ActionType type) : data_(new GroupChatData), RegularAction(type)
{
    std::shared_ptr<MainWindow> mainWindow = getGraphicManager()->getMainWindow();

    switch (type) {
    case ActionType::CREATE_GROUP:
        dataDialog_ = std::make_shared<GroupChatDataDialog>(type_, mainWindow);

        break;

    default:
        break;
    }
}

PasswordAction::PasswordAction(ActionType type) : RegularAction(type)
{
    buildDataDialog();
}

void PasswordAction::buildDataDialog()
{
    switch (type_) {
    case ActionType::CREATE_PASSWORD:
        dataDialog_ = std::make_shared<CreatePasswordDialog>();

        break;

    case ActionType::GET_PASSWORD:
        dataDialog_ = std::make_shared<GetPasswordDialog>();

        break;

    default:
        break;
    }
}

void PasswordAction::buildInfoDialog()
{
    auto function = [](TextDialog &dialog)
    {
        getGraphicManager()->disableMainWindowBlurEffect();
        dialog.closeSelfAndParentDialog();
    };

    std::string text = ""s;
    std::initializer_list<ButtonInfo> list { { ButtonType::OK, function } };

    infoDialog_ = std::make_shared<TextDialog>(
        // dataDialog_, DialogType::INFO, ""s, createOkButtonInfoVector(function));
        dataDialog_, DialogType::INFO, text, list);
}

PasswordAction::~PasswordAction()
{
    // if (type_ == ActionType::GET_PASSWORD)
    //     dataDialog_->close();
}

std::string PasswordAction::getPassword()
{
    return password_;
}

void PasswordAction::set()
{
    buildDataDialog();
    buildInfoDialog();

    auto cast = dynamic_pointer_cast<Action>(shared_from_this());

    if (!cast)
        logArgsError("action cast error");

    setDialog(cast);

    dataDialog_->show();
}

void PasswordAction::updateInfoDialog(std::shared_ptr<ActionResult> result)
{
    std::string text {};
    std::string header {};
    auto messages = std::vector<std::string> {};

    if (result->status()) {
        header = getInfoDialogSuccessHeader();
    } else {
        infoDialog_->setDialogType(DialogType::ERROR);
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
    }

    text = buildTextAsUnorderedListWithHeader(header, messages);
    infoDialog_->setText(text);
}

void PasswordAction::act()
{
    switch (type_) {
    case ActionType::CREATE_PASSWORD: {
        // auto passwordDialog = dynamic_pointer_cast<CreatePasswordDialog>(dataDialog_);

        // if (!passwordDialog)
        //     break;

        // auto text1 = passwordDialog->getFirstEditText();
        // auto text2 = passwordDialog->getSecondEditText();
        auto text1 = dataDialog_->getFirstEditText();
        auto text2 = dataDialog_->getSecondEditText();

        if (text1 == text2) {
            if (text1.size() == 0) {
                // showErrorDialogAndLogError("Empty password is invalid", getDialog());
                // showErrorDialogAndLogError("Empty password is invalid", dataDialog_);

                return;
            }

            password_ = text1;

            // getGraphicManager()->sendNewPasswordDataToManager(shared_from_this());
        }
        else {
            // showErrorDialogAndLogError("Entered passwords are not equal", getDialog());
            // showErrorDialogAndLogError("Entered passwords are not equal", dataDialog_);
        }
    }

    break;

    case ActionType::GET_PASSWORD: {
        auto passwordDialog = dynamic_pointer_cast<GetPasswordDialog>(dataDialog_);

        if (!passwordDialog)
            break;

        auto text = passwordDialog->getEditText();

        if (text.size() == 0) {
            // showErrorDialogAndLogError("Empty password is invalid", getDialog());
            // showErrorDialogAndLogError("Empty password is invalid", dataDialog_);

            return;
        }

        password_ = text;

        getGraphicManager()->sendExistingPasswordDataToManager(shared_from_this());
    }

    break;

    default:
        break;
    }

    getManager()->performAction(shared_from_this());
}

void PasswordAction::postAct(std::shared_ptr<ActionResult> result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

void PasswordAction::endAct()
{
    getManager()->endAction(shared_from_this());
}

FileAction::FileAction(
    ActionType type, const std::string &networkId, std::shared_ptr<Companion> companion)
    : Action(type)
{
    companion_ = companion;
    networkId_ = networkId;

    // auto windowTitle = getMapValue(fileDialogTypeQStringRepresentation, type, "File action"s);
    // dataDialog_ = std::make_shared<FileDialog>(shared_from_this(), windowTitle);
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
    case ActionType::SEND_FILE:
        dataDialog_->showDialog();
        break;
    case ActionType::SAVE_FILE:
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
    case ActionType::SEND_FILE:
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

    case ActionType::SAVE_FILE:
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

    default:
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
