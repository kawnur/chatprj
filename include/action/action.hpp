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

    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }
    void setDialog(ActionPtr action);

    virtual void buildDataDialog() {}
    // virtual void buildInfoDialog() {}

    virtual void set() {}
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
    std::string getInfoDialogHeader(const auto &map);
    std::string getInfoDialogSuccessHeader();
    std::string getInfoDialogFailHeader();
    // void initInfoDialog(std::initializer_list<ButtonInfo> list);
    void buildInfoDialog();

    void updateInfoDialogAndShow(
        DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list);

    void updateInfoDialogToErrorWithCloseSelfAndShow(const std::string &text);

    std::initializer_list<ButtonInfo> getOkButtonInfoCloseSelfAndParent();

protected:
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

    void set() override;
};

class CompanionAction : public RegularAction, public std::enable_shared_from_this<CompanionAction>
{
    Q_OBJECT

public:
    CompanionAction(ActionType type, CompanionPtr companion);
    ~CompanionAction();

    void buildDataDialog() override;
    // void buildInfoDialog() override;

    std::string name() override;
    std::string ipAddress() override;
    std::string serverPort() override;
    std::string clientPort() override;
    int companionId() override;
    CompanionDataPtr getCompanionData() const;
    CompanionPtr getCompanion() const;

    void set() override;

    // void updateCompanionObjectData();
    void updateInfoDialog(ActionResultPtr result);

public slots:
    void preAct() override;
    void act() override;
    void postAct(ActionResultPtr result) override;
    void endAct() override;

private:
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

    void buildDataDialog() override;
    std::string password() override;

    void set() override;
    void preAct() override;
    void updateInfoDialog(ActionResultPtr result);
    void act() override;
    void postAct(ActionResultPtr result) override;
    void endAct() override;

private:
    std::string password_;
};

class FileAction : public Action, public std::enable_shared_from_this<FileAction>
{
    Q_OBJECT

public:
    FileAction(ActionType type, const std::string &networkId, CompanionPtr companion);
    ~FileAction();

    void buildDataDialog() override;
    CompanionPtr getCompanion() const;
    std::filesystem::path getPath() override;
    void set() override;
    void preAct() override;
    void act() override;
    void defineFilePath();

private:
    std::filesystem::path filePath_;
    CompanionPtr companion_;
    std::string networkId_;
};

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines);

#endif // ACTION_HPP
