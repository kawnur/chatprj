#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "constants.hpp"
#include "manager.hpp"
#include "widgets_dialog.hpp"

class Companion;
class CompanionData;
class GroupChatData;

class Action : public QObject
{
    Q_OBJECT

public:
    Action();
    virtual ~Action();

    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }
    void setDialog(std::shared_ptr<Action> action);

    virtual void set() {}
    virtual void act() {}

    virtual ChatActionType getType() { return ChatActionType::UNKNOWN; }

protected:
    std::shared_ptr<Dialog> dialog_;
};

class RegularAction : public Action
{
    Q_OBJECT

public:
    RegularAction();
    virtual ~RegularAction();

    void set() override;
};

class CompanionAction : public RegularAction, public std::enable_shared_from_this<CompanionAction>
{
    Q_OBJECT

public:
    CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion);
    ~CompanionAction();

    ChatActionType getType() override;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<CompanionData> getCompanionData() const;
    std::shared_ptr<Companion> getCompanion() const;

    void set() override;

    void updateCompanionObjectData();

public slots:
    void act() override;

private:
    ChatActionType type_;
    std::shared_ptr<CompanionData> data_;
    std::shared_ptr<Companion> companion_;
};

class GroupChatAction : public RegularAction
{
    Q_OBJECT

public:
    GroupChatAction(ChatActionType type);
    ~GroupChatAction() = default;

private:
    ChatActionType type_;
    std::shared_ptr<GroupChatData> data_;
};

class PasswordAction : public RegularAction, public std::enable_shared_from_this<PasswordAction>
{
    Q_OBJECT

public:
    PasswordAction(PasswordActionType type);
    ~PasswordAction();

    std::string getPassword();
    void act() override;

private:
    PasswordActionType type_;
    std::string password_;
};

class FileAction : public Action, public std::enable_shared_from_this<FileAction>
{
    Q_OBJECT

public:
    FileAction(
        FileActionType type, const std::string &networkId,
        std::shared_ptr<Companion> companion);

    ~FileAction() = default;

    std::shared_ptr<Companion> getCompanion() const;
    std::filesystem::path getPath() const;
    void set() override;

    void act() override;
    void defineFilePath();

private:
    FileActionType type_;
    std::filesystem::path filePath_;
    std::shared_ptr<Companion> companion_;
    std::string networkId_;
};

#endif // ACTION_HPP
