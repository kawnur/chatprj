#include "action.hpp"

#include <string>

#include "action_result.hpp"
#include "action_wrapper.hpp"
#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "logging.hpp"
#include "manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

Action::Action(ActionType type) : type_(type) {}

Action::~Action() { logArgsInfo(Q_FUNC_INFO); }

void Action::setDialog(ActionPtr action)
{
    if (dataDialog_)
        dataDialog_->set(action);

    if (infoDialog_)
        infoDialog_->set(action);
}

ActionType Action::type()
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

void Action::buildInfoDialog()
{
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

CompanionAction::CompanionAction(ActionType type, CompanionPtr companion)
    : companion_(companion), data_(nullptr), RegularAction(type) {}

CompanionAction::~CompanionAction() { logArgsInfo(Q_FUNC_INFO); }

void CompanionAction::buildDataDialog()
{
    switch (type_) {
    case ActionType::CREATE_COMPANION:
    case ActionType::UPDATE_COMPANION:
        dataDialog_ = std::make_shared<CompanionDataDialog>(type_, companion_);

    break;

    default:
        break;
    }
}

std::string CompanionAction::name()
{
    return data_->name();
}

std::string CompanionAction::ipAddress()
{
    return data_->ipAddress();
}

std::string CompanionAction::serverPort()
{
    return data_->serverPort();
}

std::string CompanionAction::clientPort()
{
    return data_->clientPort();
}

int CompanionAction::companionId()
{
    return companion_->id();
}

CompanionDataPtr CompanionAction::getCompanionData() const
{
    return data_;
}

CompanionPtr CompanionAction::getCompanion() const
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

// void CompanionAction::updateCompanionObjectData()
// {
//     // companion_->updateData(data_);
//     getManager()->updateCompanion()
// }

void CompanionAction::updateInfoDialog(ActionResultPtr result)
{
    // TODO move to parent
    DialogType type = DialogType::UNKNOWN;
    std::string text {};
    std::string header {};
    auto messages = std::vector<std::string> {};

    if (result->status()) {
        type = DialogType::INFO;

        std::vector<std::pair<std::string, std::string>> lines {
            { "name: {}", data_->name() },
            { "ipAddress: {}", data_->ipAddress() },
            { "port: {}", data_->clientPort() }
        };

        header = getInfoDialogSuccessHeader();
        fillMessages(messages, lines);

    } else {
        type = DialogType::ERROR;
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
    }

    text = buildTextAsUnorderedListWithHeader(header, messages);
    auto list = { getOKButtonInfo([this]() { infoDialog_->closeSelfAndParent(); }) };
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
        auto list = { getOKButtonInfo([this]() { act(); }), getCancelButtonInfo([this]() { endAct(); }) };
        updateInfoDialogAndShow(DialogType::WARNING, deleteCompanionDialogText, list);
    }

    break;
    case ActionType::CLEAR_HISTORY: {
        auto list = { getOKButtonInfo([this]() { act(); }), getCancelButtonInfo([this]() { endAct(); }) };
        updateInfoDialogAndShow(DialogType::WARNING, clearCompanionHistoryDialogText, list);
    }

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

        auto name = cast->name();
        auto address = cast->ipAddress();
        auto port = cast->port();

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
        data_ = getManager()->getCompanionData(companion_);

    break;

    case ActionType::SEND_HISTORY:
        // TODO if client is disconnected show error dialog
        getManager()->sendHistoryToCompanion(companion_);

    break;

    default:
        break;
    }

    data_->log();

    getManager()->performAction(shared_from_this());
}

void CompanionAction::postAct(ActionResultPtr result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

void CompanionAction::endAct()
{
    getManager()->endAction(shared_from_this());
}

GroupChatAction::GroupChatAction(ActionType type) : data_(), RegularAction(type)
{
    switch (type) {
    case ActionType::CREATE_GROUP:
        dataDialog_ = std::make_shared<GroupChatDataDialog>(type_);

        break;

    default:
        break;
    }
}

PasswordAction::PasswordAction(ActionType type) : RegularAction(type) {}

PasswordAction::~PasswordAction() { logArgsInfo(Q_FUNC_INFO); }

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

std::string PasswordAction::password()
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

void PasswordAction::updateInfoDialog(ActionResultPtr result)
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
            getManager()->disableBlur();
            infoDialog_->closeSelfAndParent();
        };
    } else {
        type = DialogType::ERROR;
        header = getInfoDialogFailHeader();
        messages.push_back(result->definition());
        text = buildTextAsUnorderedListWithHeader(header, messages);

        function = [this]()
        {
            infoDialog_->closeSelfAndParent();
        };
    }

    auto list = { getOKButtonInfo(function) };
    infoDialog_->update(type, text, list);
}

void PasswordAction::act()
{
    switch (type_) {
    case ActionType::CREATE_PASSWORD: {
        auto text1 = dataDialog_->firstEditText();
        auto text2 = dataDialog_->secondEditText();

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
        auto text = dataDialog_->editText();

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

void PasswordAction::postAct(ActionResultPtr result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

void PasswordAction::endAct()
{
    getManager()->endAction(shared_from_this());
}

FileAction::FileAction(
    ActionType type, const std::string &networkId, CompanionPtr companion)
    : Action(type), companion_(companion), networkId_(networkId) {}

FileAction::~FileAction() { logArgsInfo(Q_FUNC_INFO); }

void FileAction::buildDataDialog()
{
    dataDialog_ = std::make_shared<FileDialog>(type_);
}

CompanionPtr FileAction::getCompanion() const
{
    return companion_;
}

std::filesystem::path FileAction::getPath()
{
    return filePath_;
}

void FileAction::set()
{
    buildDataDialog();
    buildInfoDialog();

    auto cast = dynamic_pointer_cast<Action>(shared_from_this());

    if (!cast)
        logArgsError("action cast error");

    setDialog(cast);
    preAct();
}

void FileAction::preAct()
{
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
    // auto cast = dynamic_pointer_cast<FileDialog>(dataDialog_);

    // if (!cast)
    //     return;

    // auto dialog = cast->getFileDialog();

    // if (!dialog)
    //     return;

    auto manager = getManager();

    switch (type_) {
    case ActionType::SEND_FILE:
    {
        for (const auto &path : dataDialog_->selectedFiles()) {  // one file
            filePath_ = std::filesystem::path(getString(path));

            manager->sendMessage(
                MessageType::FILE, companion_, shared_from_this(), filePath_.filename().string());

            manager->setLastOpenedPath(filePath_.parent_path());
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
        // bool setResult = companion_->setFileOperatorFilePath(networkId_, filePath_);
        bool setResult = manager->setFileOperatorFilePath(networkId_, filePath_);

        if (setResult) {
            // send without saving to db
            // auto meta = std::make_shared<MessageMetaData>();
            // meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;
            // meta->networkId_ = networkId_;

            // bool result = manager->sendMessage(companion_, nullptr, meta);
            bool result = manager->sendFileRequestMessage(companion_, networkId_);
            manager->setLastOpenedPath(filePath_.parent_path());
        }
        else {
            logTemplateError("error saving file, path: {}", filePath_);
        }
    }

    break;

    default:
        break;
    }
}

void FileAction::defineFilePath()
{
    // auto pathQStr = QString::fromStdString(getManager()->getLastOpenedPath().string());
    auto pathQStr = getQString(getManager()->getLastOpenedPath());
    QString param { "Save File" };
    auto result = dataDialog_->getSaveFileName(param, pathQStr);
    filePath_ = std::filesystem::path(result.toStdString());
    act();
}

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines)
{
    for (const auto &line : lines)
        messages.emplace_back(getStringByFormat(line.first, line.second));
}
