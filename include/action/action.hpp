#ifndef ACTION_HPP
#define ACTION_HPP

#include <array>
#include <memory>
#include <filesystem>

#include <QObject>

#include "alias.hpp"
#include "constants.hpp"
#include "widgets_dialog.hpp"

using Headers = std::array<std::string, 2>;

class Action : public QObject, public std::enable_shared_from_this<Action>
{
    Q_OBJECT

public:
    Action(ManagerPtr manager, const std::string &title);
    virtual ~Action();

    void setDialog(ActionPtr action);

    virtual void setDataDialog() {}
    virtual void preAct() {}
    virtual ActionResultPtr act() { return nullptr; }
    virtual void postAct(ActionResultPtr result) {}
    virtual std::string name() { return ""s; }
    virtual std::string ipAddress() { return ""s; }
    virtual std::string serverPort() { return ""s; }
    virtual std::string clientPort() { return ""s; }
    virtual int companionId() { return ID_BAD_VALUE; }
    virtual std::string password() { return ""s; }
    virtual std::filesystem::path getPath() { return std::filesystem::path(); }

public slots:
    void perform();
    void endAct();

protected:
    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }

    template<typename T>
    std::shared_ptr<T> getShared()
    {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }

    void updateInfoDialogBase(
        bool status, const std::vector<std::string> &messages,
        std::initializer_list<ButtonInfo> list);

    void updateInfoDialogAndShow(
        DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list);

    void updateInfoDialogToErrorWithCloseSelfAndShow(const std::string &text);

    ManagerPtr manager_;
    std::shared_ptr<TextDialog> infoDialog_;
    std::string dialogTitle_;
    std::array<std::string, 2> infoDialogHeaders_;
};

class CompanionAction : public Action
{
    Q_OBJECT

public:
    CompanionAction(ManagerPtr manager, CompanionPtr companion, const std::string &title);
    virtual ~CompanionAction();

    std::string name() override;
    std::string ipAddress() override;
    std::string serverPort() override;
    std::string clientPort() override;
    int companionId() override;
    CompanionDataPtr getCompanionData() const;
    CompanionPtr getCompanion() const;

    void postAct(ActionResultPtr result) override;

protected:
    void setDataDialog() override;
    void updateData();
    void updateInfoDialog(ActionResultPtr result);

    CompanionDataPtr data_;
    CompanionPtr companion_;
    std::shared_ptr<CompanionDataDialog> dataDialog_;
};

class CreateCompanionAction : public CompanionAction
{
public:
    CreateCompanionAction(ManagerPtr manager);
    ~CreateCompanionAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

class UpdateCompanionAction : public CompanionAction
{
public:
    UpdateCompanionAction(ManagerPtr manager, CompanionPtr companion);
    ~UpdateCompanionAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

class DeleteCompanionAction : public CompanionAction
{
public:
    DeleteCompanionAction(ManagerPtr manager, CompanionPtr companion);
    ~DeleteCompanionAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

class ClearHistoryCompanionAction : public CompanionAction
{
public:
    ClearHistoryCompanionAction(ManagerPtr manager, CompanionPtr companion);
    ~ClearHistoryCompanionAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

class SendHistoryCompanionAction : public CompanionAction
{
public:
    SendHistoryCompanionAction(ManagerPtr manager, CompanionPtr companion);
    ~SendHistoryCompanionAction() = default;

    ActionResultPtr act() override;
};

// class GroupChatAction : public RegularAction
// {
//     Q_OBJECT

// public:
//     GroupChatAction();
//     ~GroupChatAction() = default;

// private:
//     std::shared_ptr<GroupChatData> data_;
// };

class PasswordAction : public Action
{
    Q_OBJECT

public:
    PasswordAction(ManagerPtr manager, const std::string &title);
    ~PasswordAction();

    std::string password() override;

    void preAct() override;
    void postAct(ActionResultPtr result) override;

protected:
    void setDataDialog() override;
    void updateInfoDialog(ActionResultPtr result);

    std::string password_;
    std::shared_ptr<DataDialog> dataDialog_;
};

class CreatePasswordAction : public PasswordAction
{
public:
    CreatePasswordAction(ManagerPtr manager);
    ~CreatePasswordAction() = default;

    ActionResultPtr act() override;
};

class GetPasswordAction : public PasswordAction
{
public:
    GetPasswordAction(ManagerPtr manager);
    ~GetPasswordAction() = default;

    ActionResultPtr act() override;
};

class FileAction : public Action
{
    Q_OBJECT

public:
    FileAction(
        ManagerPtr manager, const std::string &networkId, CompanionPtr companion,
        const std::string &title);

    ~FileAction();

    std::filesystem::path getPath() override;

protected:
    void setDataDialog() override;
    void defineFilePath();

    std::shared_ptr<FileDialog> dataDialog_;
    std::filesystem::path filePath_;
    CompanionPtr companion_;
    std::string networkId_;
};

class SendFileAction : public FileAction
{
public:
    SendFileAction(ManagerPtr manager, const std::string &networkId, CompanionPtr companion);
    ~SendFileAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

class SaveFileAction : public FileAction
{
public:
    SaveFileAction(ManagerPtr manager, const std::string &networkId, CompanionPtr companion);
    ~SaveFileAction() = default;

    void preAct() override;
    ActionResultPtr act() override;
};

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines);

#endif // ACTION_HPP
