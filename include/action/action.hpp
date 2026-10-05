#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "constants.hpp"
// #include "manager.hpp"
#include "widgets_dialog.hpp"

class ActionResult;
class Companion;
class CompanionData;
class GroupChatData;

class Action : public QObject
{
    Q_OBJECT

public:
    Action(ActionType type);
    virtual ~Action();

    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }
    void setDialog(std::shared_ptr<Action> action);

    virtual void buildDataDialog() {}
    virtual void buildInfoDialog() {}

    virtual void set() {}
    virtual void act() {}
    virtual void postAct(std::shared_ptr<ActionResult> result) {}
    virtual void endAct() {}

    ActionType getType();
    std::string getInfoDialogHeader(const auto &map);
    std::string getInfoDialogSuccessHeader();
    std::string getInfoDialogFailHeader();

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
    // CompanionAction(ActionType type, std::shared_ptr<Companion> companion = nullptr);
    CompanionAction(ActionType type, std::shared_ptr<Companion> companion);
    ~CompanionAction();

    void buildDataDialog() override;
    void buildInfoDialog() override;

    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<CompanionData> getCompanionData() const;
    std::shared_ptr<Companion> getCompanion() const;

    void set() override;

    void updateCompanionObjectData();
    void updateInfoDialog(std::shared_ptr<ActionResult> result);

public slots:
    void act() override;
    void postAct(std::shared_ptr<ActionResult> result) override;
    void endAct() override;

private:
    std::shared_ptr<CompanionData> data_;
    std::shared_ptr<Companion> companion_;
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
    void buildInfoDialog() override;

    std::string getPassword();

    void set() override;
    void updateInfoDialog(std::shared_ptr<ActionResult> result);
    void act() override;
    void postAct(std::shared_ptr<ActionResult> result) override;
    void endAct() override;

private:
    std::string password_;
};

class FileAction : public Action, public std::enable_shared_from_this<FileAction>
{
    Q_OBJECT

public:
    FileAction(ActionType type, const std::string &networkId, std::shared_ptr<Companion> companion);
    ~FileAction() = default;

    std::shared_ptr<Companion> getCompanion() const;
    std::filesystem::path getPath() const;
    void set() override;

    void act() override;
    void defineFilePath();

private:
    ActionType type_;
    std::filesystem::path filePath_;
    std::shared_ptr<Companion> companion_;
    std::string networkId_;
};

void fillMessages(
    std::vector<std::string> &messages, std::vector<std::pair<std::string, std::string>> lines);

#endif // ACTION_HPP
