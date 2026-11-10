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

Action::Action(ManagerPtr manager, const std::string &title)
    : manager_(manager), dialogTitle_(title)
{
    std::initializer_list<ButtonInfo> list {};
    infoDialog_ = std::make_shared<TextDialog>(nullptr, dialogTitle_, ""s, list);
}

Action::~Action() { logArgsInfo(Q_FUNC_INFO); }

void Action::setDialog(ActionPtr action)
{
    setDataDialog();

    if (infoDialog_)
        infoDialog_->set(action);
}

void Action::perform()
{
    auto result = act();
    postAct(result);
}

void Action::endAct()
{
    manager_->endAction(shared_from_this());
}

void Action::updateInfoDialogBase(
    bool status, const std::vector<std::string> &messages, std::initializer_list<ButtonInfo> list)
{
    auto lambda = [this](bool status) { return infoDialogHeaders_.at(uint(status)); };
    auto header = runAndReturnDefault(lambda, ""s, status);
    auto text = buildTextAsUnorderedListWithHeader(header, messages);
    infoDialog_->update(text, list);
}

void Action::updateInfoDialogAndShow(
    const std::string &text, std::initializer_list<ButtonInfo> list)
{
    infoDialog_->update(text, list);
    infoDialog_->show();
}

void Action::updateInfoDialogToErrorWithCloseSelfAndShow(const std::string &text)
{
    auto list = { getOKButtonInfo([this]() { infoDialog_->closeSelf(); }) };
    updateInfoDialogAndShow(text, list);
}

CompanionAction::CompanionAction(
    ManagerPtr manager, CompanionPtr companion, const std::string &title)
    : Action(manager, title), companion_(companion), data_(nullptr)
{
    infoDialog_->setParent(dataDialog_.get());
}

CompanionAction::~CompanionAction() { logArgsInfo(Q_FUNC_INFO); }

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

void CompanionAction::updateInfoDialog(ActionResultPtr result)
{
    auto status = result->status();
    auto messages = std::vector<std::string> {};

    if (status) {
        std::vector<std::pair<std::string, std::string>> lines {
            { "name: {}", data_->name() },
            { "ipAddress: {}", data_->ipAddress() },
            { "port: {}", data_->clientPort() }
        };

        fillMessages(messages, lines);
    } else {
        messages.push_back(result->definition());
    }

    auto list = { getOKButtonInfo([this]() { infoDialog_->closeSelfAndParent(); }) };
    updateInfoDialogBase(status, messages, list);
}

void CompanionAction::postAct(ActionResultPtr result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

void CompanionAction::setDataDialog()
{
    if (dataDialog_)
        dataDialog_->set(shared_from_this());
}

void CompanionAction::updateData()
{
    // set data field
    auto data = dataDialog_->getCompanionData();

    if (!data->checkAsUpdateResult()) {
        updateInfoDialogToErrorWithCloseSelfAndShow(EMPTY_FIELD_DIALOG_TEXT);

        return;
    }

    data_ = data;
}

CreateCompanionAction::CreateCompanionAction(ManagerPtr manager)
    : CompanionAction(manager, nullptr, "Add new companion")
{
    dataDialog_ = std::make_shared<CompanionDataDialog>(companion_, dialogTitle_);
    infoDialogHeaders_ = { "Companion addition error", "New companion added" };
}

void CreateCompanionAction::preAct()
{
    dataDialog_->show();
}

ActionResultPtr CreateCompanionAction::act()
{
    updateData();
    data_->log();

    return manager_->createCompanion(getShared<CreateCompanionAction>());
}

UpdateCompanionAction::UpdateCompanionAction(ManagerPtr manager, CompanionPtr companion)
    : CompanionAction(manager, companion, "Edit companion")
{
    dataDialog_ = std::make_shared<UpdateCompanionDataDialog>(companion_, dialogTitle_);
    infoDialogHeaders_ = { "Companion edition error", "Companion edited" };
}

void UpdateCompanionAction::preAct()
{
    dataDialog_->show();
}

ActionResultPtr UpdateCompanionAction::act()
{
    updateData();
    data_->log();

    return manager_->updateCompanion(getShared<UpdateCompanionAction>());
}

DeleteCompanionAction::DeleteCompanionAction(ManagerPtr manager, CompanionPtr companion)
    : CompanionAction(manager, companion, "Delete companion")
{
    infoDialogHeaders_ = { "Companion deletion error", "Companion deleted" };
}

void DeleteCompanionAction::preAct()
{
    auto list = {
        getOKButtonInfo([this]() { perform(); }),
        getCancelButtonInfo([this]() { endAct(); })
    };

    updateInfoDialogAndShow(deleteCompanionDialogText, list);
}

ActionResultPtr DeleteCompanionAction::act()
{
    data_ = getManager()->getCompanionData(companion_);
    data_->log();

    return manager_->deleteCompanion(getShared<DeleteCompanionAction>());
}

ClearHistoryCompanionAction::ClearHistoryCompanionAction(ManagerPtr manager, CompanionPtr companion)
    : CompanionAction(manager, companion, "Clear companion history")
{
    infoDialogHeaders_ = { "Chat history clearing error", "Chat history cleared" };
}

void ClearHistoryCompanionAction::preAct()
{
    // TODO move to base
    auto list = {
        getOKButtonInfo([this]() { perform(); }),
        getCancelButtonInfo([this]() { endAct(); })
    };

    updateInfoDialogAndShow(clearCompanionHistoryDialogText, list);
}

ActionResultPtr ClearHistoryCompanionAction::act()
{
    data_ = getManager()->getCompanionData(companion_);
    data_->log();

    return manager_->clearCompanionHistory(getShared<ClearHistoryCompanionAction>());
}

SendHistoryCompanionAction::SendHistoryCompanionAction(ManagerPtr manager, CompanionPtr companion)
    : CompanionAction(manager, companion, "Send companion history") {}

ActionResultPtr SendHistoryCompanionAction::act()
{
    // TODO if client is disconnected show error dialog
    getManager()->sendHistoryToCompanion(companion_);
    data_->log();

    // add manager action
}

// GroupChatAction::GroupChatAction(ActionType type) : data_(), RegularAction(type)
// {
//     buildDataDialog();

//     switch (type) {
//     case ActionType::CREATE_GROUP:
//         dataDialog_ = std::make_shared<GroupChatDataDialog>(type_);

//         break;

//     default:
//         break;
//     }
// }

PasswordAction::PasswordAction(ManagerPtr manager, const std::string &title)
    : Action(manager, title)
{
    infoDialog_->setParent(dataDialog_.get());
}

PasswordAction::~PasswordAction() { logArgsInfo(Q_FUNC_INFO); }

std::string PasswordAction::password()
{
    return password_;
}

void PasswordAction::preAct()
{
    dataDialog_->show();
}

void PasswordAction::setDataDialog()
{
    dataDialog_->set(shared_from_this());
}

void PasswordAction::updateInfoDialog(ActionResultPtr result)
{
    auto status = result->status();
    auto messages = std::vector<std::string> {};
    std::function<void()> function;

    if (status) {
        function = [this]()
        {
            getManager()->disableBlur();
            infoDialog_->closeSelfAndParent();
        };
    } else {
        messages.push_back(result->definition());

        function = [this]()
        {
            infoDialog_->closeSelfAndParent();
        };
    }

    auto list = { getOKButtonInfo(function) };
    updateInfoDialogBase(status, messages, list);
}

void PasswordAction::postAct(ActionResultPtr result)
{
    updateInfoDialog(result);
    infoDialog_->show();
}

CreatePasswordAction::CreatePasswordAction(ManagerPtr manager)
    : PasswordAction(manager, "Create new password")
{
    auto list = { DataType::PASSWORD, DataType::PASSWORD_REENTER };
    dataDialog_ = std::make_shared<DataDialog>(dialogTitle_, list);
    infoDialogHeaders_ = { "Password creation error", "New password created" };
}

ActionResultPtr CreatePasswordAction::act()
{
    auto data = dataDialog_->getData();
    auto text1 = data->get(DataType::PASSWORD);
    auto text2 = data->get(DataType::PASSWORD_REENTER);

    auto errorText = ""s;

    if (text1 != text2)
        errorText = PASSWORDS_ARE_NOT_EQUAL_DIALOG_TEXT;
    else if (text1.size() == 0)
        errorText = EMPTY_FIELD_DIALOG_TEXT;

    if (!errorText.empty()) {
        updateInfoDialogToErrorWithCloseSelfAndShow(errorText);

        return std::make_shared<ActionResult>(false, ""s);
    }

    password_ = text1;

    return manager_->createUserPassword(getShared<CreatePasswordAction>());
}

GetPasswordAction::GetPasswordAction(ManagerPtr manager)
    : PasswordAction(manager, "Authentication")
{
    auto list = { DataType::PASSWORD };
    dataDialog_ = std::make_shared<DataDialog>(dialogTitle_, list);
    infoDialogHeaders_ = { "User authentication error", "User successfully authenticated" };
}

ActionResultPtr GetPasswordAction::act()
{
    auto data = dataDialog_->getData();
    auto text = data->get(DataType::PASSWORD);

    if (text.size() == 0) {
        updateInfoDialogToErrorWithCloseSelfAndShow(EMPTY_FIELD_DIALOG_TEXT);

        return std::make_shared<ActionResult>(false, ""s);
    }

    password_ = text;

    return manager_->authenticateUser(getShared<GetPasswordAction>());
}

FileAction::FileAction(
    ManagerPtr manager, const std::string &networkId, CompanionPtr companion,
    const std::string &title)
    : Action(manager, title), companion_(companion), networkId_(networkId)
{
    dataDialog_ = std::make_shared<FileDialog>(title);
}

FileAction::~FileAction() { logArgsInfo(Q_FUNC_INFO); }

std::filesystem::path FileAction::getPath()
{
    return filePath_;
}

void FileAction::setDataDialog()
{
    dataDialog_->set(shared_from_this());
}

void FileAction::defineFilePath()
{
    auto pathQStr = getQString(getManager()->getLastOpenedPath());
    QString param { "Save File" };
    auto result = dataDialog_->getSaveFileName(param, pathQStr);
    filePath_ = std::filesystem::path(result.toStdString());
    act();
}

SendFileAction::SendFileAction(
    ManagerPtr manager, const std::string &networkId, CompanionPtr companion)
    : FileAction(manager, networkId, companion, "Send file") {}

void SendFileAction::preAct()
{
    dataDialog_->showDialog();
}

ActionResultPtr SendFileAction::act()
{
    for (const auto &path : dataDialog_->selectedFiles()) {
        filePath_ = std::filesystem::path(getString(path));
        auto text = filePath_.filename().string();
        manager_->setLastOpenedPath(filePath_.parent_path());
    }
}

SaveFileAction::SaveFileAction(
    ManagerPtr manager, const std::string &networkId, CompanionPtr companion)
    : FileAction(manager, networkId, companion, "Save file") {}

void SaveFileAction::preAct()
{
    defineFilePath();
}

ActionResultPtr SaveFileAction::act()
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
    bool setResult = manager_->setFileOperatorFilePath(networkId_, filePath_);

    if (setResult) {
        // send without saving to db
        // auto meta = std::make_shared<MessageMetaData>();
        // meta->networkMessageType_ = NetworkMessageType::FILE_REQUEST;
        // meta->networkId_ = networkId_;

        // bool result = manager->sendMessage(companion_, nullptr, meta);
        bool result = manager_->sendFileRequestMessage(companion_, networkId_);
        manager_->setLastOpenedPath(filePath_.parent_path());
    }
    else {
        logTemplateError("error saving file, path: {}", filePath_);
    }
}

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines)
{
    for (const auto &line : lines)
        messages.emplace_back(getStringByFormat(line.first, line.second));
}
