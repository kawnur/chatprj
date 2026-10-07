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
    if (dataDialog_)
        dataDialog_->set(action);

    if (infoDialog_)
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

// void Action::initInfoDialog(std::initializer_list<ButtonInfo> list)
void Action::buildInfoDialog()
{
    // infoDialog_ = std::make_shared<TextDialog>(dataDialog_, DialogType::INFO, ""s, list);
    auto list = { ButtonInfo() };
    infoDialog_ = std::make_shared<TextDialog>(dataDialog_, DialogType::INFO, ""s, list);
}

void Action::updateInfoDialogAndShow(
    DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list)
{
    infoDialog_->update(type, text, list);
    infoDialog_->show();
}

void Action::updateInfoDialogToErrorWithCloseSelfAndShow(const std::string &text)
{
    auto list = { getOKButtonInfo([this]() { infoDialog_->closeSelf(); }) };
    updateInfoDialogAndShow(DialogType::ERROR, text, list);
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
        // std::initializer_list<ButtonInfo> list {
        //     // { ButtonType::DELETE_COMPANION, [](TextDialog &dialog) { dialog.acceptAction(); } }
        //     { ButtonType::DELETE_COMPANION, [this]() { infoDialog_->acceptAction(); } }
        // };

        // dataDialog_ = std::make_shared<TextDialog>(
        //     nullptr, DialogType::WARNING, deleteCompanionDialogText,
        //     // getButtonInfoVector(ButtonType::DELETE_COMPANION));
        //     list);
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

// void CompanionAction::buildInfoDialog()
// {
//     initInfoDialog({ getOKButtonInfo([this]() { infoDialog_->closeSelfAndParentDialog(); }) });
// }

std::string CompanionAction::getName()
{
    return data_->getName();
}

std::string CompanionAction::getIpAddress()
{
    return data_->getIpAddress();
}

std::string CompanionAction::getServerPort()
{
    return data_->getServerPort();
}

std::string CompanionAction::getClientPort()
{
    return data_->getClientPort();
}

int CompanionAction::getCompanionId()
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
    preAct();
}

void CompanionAction::updateCompanionObjectData()
{
    companion_->updateData(data_);
}

void CompanionAction::updateInfoDialog(std::shared_ptr<ActionResult> result)
{
    // TODO move to parent
    DialogType type = DialogType::UNKNOWN;
    std::string text {};
    std::string header {};
    auto messages = std::vector<std::string> {};

    if (result->status()) {
        type = DialogType::INFO;

        std::vector<std::pair<std::string, std::string>> lines {
            { "name: {}", data_->getName() },
            { "ipAddress: {}", data_->getIpAddress() },
            { "port: {}", data_->getClientPort() }
        };

        header = getInfoDialogSuccessHeader();
        fillMessages(messages, lines);

    } else {
        type = DialogType::ERROR;
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
    }

    text = buildTextAsUnorderedListWithHeader(header, messages);
    auto list = { getOKButtonInfo([this]() { infoDialog_->closeSelfAndParentDialog(); }) };
    infoDialog_->update(type, text, list);
}

void CompanionAction::preAct()
{
    switch (type_) {
    case ActionType::CREATE_COMPANION:
    case ActionType::UPDATE_COMPANION:
        dataDialog_->show();

    break;

    case ActionType::DELETE_COMPANION: {
        auto list = { getOKButtonInfo([this]() { act(); }) };
        updateInfoDialogAndShow(DialogType::WARNING, deleteCompanionDialogText, list);
    }

    break;
    case ActionType::CLEAR_HISTORY:
        // data_ = std::make_shared<CompanionData>(
        //     companion_->getName(), companion_->getSocketIpAddress(), ""s,
        //     std::to_string(companion_->getSocketClientPort()));

        break;

    case ActionType::SEND_HISTORY:
        // // TODO if client is disconnected show error dialog
        // getManager()->sendChatHistoryToCompanion(companion_);

        break;

    default:
        break;
    }

}

void CompanionAction::act()
{
    switch (type_) {
    case ActionType::CREATE_COMPANION:
    case ActionType::UPDATE_COMPANION: {
        auto cast = dynamic_pointer_cast<CompanionDataDialog>(dataDialog_);

        if (!cast)
            break;

        auto name = cast->getNameString();
        auto address = cast->getIpAddressString();
        auto port = cast->getPortString();

        // check dialog data
        for (const auto &item : { name, address, port }) {
            if (item.empty()) {
                updateInfoDialogToErrorWithCloseSelfAndShow(EMPTY_FIELD_DIALOG_TEXT);

                return;
            }
        }

        // set data field
        data_ = std::make_shared<CompanionData>(name, address, ""s, port);
    }

    break;

    case ActionType::DELETE_COMPANION:
    case ActionType::CLEAR_HISTORY:
        data_ = std::make_shared<CompanionData>(
            companion_->getName(), companion_->getSocketIpAddress(), ""s,
            std::to_string(companion_->getSocketClientPort()));

    break;

    case ActionType::SEND_HISTORY:
        // TODO if client is disconnected show error dialog
        getManager()->sendChatHistoryToCompanion(companion_);

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

// void PasswordAction::buildInfoDialog()
// {
//     auto function = [this]()
//     {
//         getGraphicManager()->disableMainWindowBlurEffect();
//         infoDialog_->closeSelfAndParentDialog();
//     };

//     initInfoDialog({ getOKButtonInfo(function) });
// }

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
    preAct();
}

void PasswordAction::preAct()
{
    dataDialog_->show();
}

void PasswordAction::updateInfoDialog(std::shared_ptr<ActionResult> result)
{
    // TODO move to parent
    DialogType type = DialogType::UNKNOWN;
    std::string text {};
    std::string header {};
    auto messages = std::vector<std::string> {};
    std::function<void()> function;

    if (result->status()) {
        type = DialogType::INFO;
        text = getInfoDialogSuccessHeader();

        function = [this]()
        {
            getGraphicManager()->disableMainWindowBlurEffect();
            infoDialog_->closeSelfAndParentDialog();
        };
    } else {
        type = DialogType::ERROR;
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
        text = buildTextAsUnorderedListWithHeader(header, messages);

        function = [this]()
        {
            infoDialog_->closeSelfAndParentDialog();
        };
    }

    auto list = { getOKButtonInfo(function) };
    infoDialog_->update(type, text, list);
}

void PasswordAction::act()
{
    switch (type_) {
    case ActionType::CREATE_PASSWORD: {
        auto text1 = dataDialog_->getFirstEditText();
        auto text2 = dataDialog_->getSecondEditText();

        auto errorText = ""s;

        if (text1 != text2)
            errorText = PASSWORDS_ARE_NOT_EQUAL_DIALOG_TEXT;
        else if (text1.size() == 0)
            errorText = EMPTY_FIELD_DIALOG_TEXT;

        if (!errorText.empty()) {
            updateInfoDialogToErrorWithCloseSelfAndShow(errorText);

            return;
        }

        password_ = text1;
    }

    break;

    case ActionType::GET_PASSWORD: {
        auto text = dataDialog_->getEditText();

        if (text.size() == 0) {
            updateInfoDialogToErrorWithCloseSelfAndShow(EMPTY_FIELD_DIALOG_TEXT);

            return;
        }

        password_ = text;
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
