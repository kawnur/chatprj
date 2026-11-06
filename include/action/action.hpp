#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "alias.hpp"
#include "constants.hpp"
#include "widgets_dialog.hpp"

class Action : public QObject
{
    Q_OBJECT

public:
    Action(ActionType type);
    virtual ~Action();

    void setDialog(ActionPtr action);

    virtual void preAct() {}
    virtual void act() {}
    virtual void postAct(ActionResultPtr result) {}
    virtual void endAct() {}

    virtual std::string name() { return ""s; }
    virtual std::string ipAddress() { return ""s; }
    virtual std::string serverPort() { return ""s; }
    virtual std::string clientPort() { return ""s; }
    virtual int companionId() { return ID_BAD_VALUE; }
    virtual std::string password() { return ""s; }
    virtual std::filesystem::path getPath() { return std::filesystem::path(); }

    ActionType type();

protected:
    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }
    virtual void buildDataDialog() {}

    void buildInfoDialog();
    std::string getInfoDialogHeader(DialogType type);

    void updateInfoDialogBase(
        bool status, const std::vector<std::string> &messages,
        std::initializer_list<ButtonInfo> list);

    void updateInfoDialogAndShow(
        DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list);

    void updateInfoDialogToErrorWithCloseSelfAndShow(const std::string &text);

    ActionType type_;
    std::shared_ptr<Dialog> dataDialog_;
    std::shared_ptr<TextDialog> infoDialog_;
};

class RegularAction : public Action
{
    Q_OBJECT

public:
    RegularAction(ActionType type);
    virtual ~RegularAction();
};

class CompanionAction : public RegularAction, public std::enable_shared_from_this<CompanionAction>
{
    Q_OBJECT

public:
    CompanionAction(ActionType type, CompanionPtr companion);
    ~CompanionAction();

    std::string name() override;
    std::string ipAddress() override;
    std::string serverPort() override;
    std::string clientPort() override;
    int companionId() override;
    CompanionDataPtr getCompanionData() const;
    CompanionPtr getCompanion() const;

public slots:
    void preAct() override;
    void act() override;
    void postAct(ActionResultPtr result) override;
    void endAct() override;

private:
    void buildDataDialog() override;
    void updateInfoDialog(ActionResultPtr result);

    CompanionDataPtr data_;
    CompanionPtr companion_;
};

class GroupChatAction : public RegularAction
{
    Q_OBJECT

public:
    GroupChatAction(ActionType type);
    ~GroupChatAction() = default;

private:
    std::shared_ptr<GroupChatData> data_;
};

class PasswordAction : public RegularAction, public std::enable_shared_from_this<PasswordAction>
{
    Q_OBJECT

public:
    PasswordAction(ActionType type);
    ~PasswordAction();

    std::string password() override;

    void preAct() override;
    void act() override;
    void postAct(ActionResultPtr result) override;
    void endAct() override;

private:
    void buildDataDialog() override;
    void updateInfoDialog(ActionResultPtr result);

    std::string password_;
};

class FileAction : public Action, public std::enable_shared_from_this<FileAction>
{
    Q_OBJECT

public:
    FileAction(ActionType type, const std::string &networkId, CompanionPtr companion);
    ~FileAction();

    std::filesystem::path getPath() override;
    void preAct() override;
    void act() override;

private:
    void buildDataDialog() override;
    void defineFilePath();

    std::filesystem::path filePath_;
    CompanionPtr companion_;
    std::string networkId_;
};

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines);

#endif // ACTION_HPP
