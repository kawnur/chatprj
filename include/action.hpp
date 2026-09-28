#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "constants.hpp"

class Action;
class Dialog;
class Companion;
class CompanionData;
class GroupChatData;

// class DialogWrapper : public std::enable_shared_from_this<DialogWrapper>
// {
// public:
//     DialogWrapper(std::shared_ptr<Dialog> dialog);
//     ~DialogWrapper() = default;

//     std::shared_ptr<Dialog> getDialog();
//     void set(std::shared_ptr<Action> action);
//     void showDialog();
//     void act();

// private:
//     std::shared_ptr<Action> action_;
//     std::shared_ptr<Dialog> dialog_;
// };

// class Action : public QObject, public std::enable_shared_from_this<Action>
class Action
{
    // Q_OBJECT

public:
    // Action(std::shared_ptr<Dialog> dialog);
    Action();
    ~Action() = default;

    std::shared_ptr<Dialog> getDialog();
    void setDialog(std::shared_ptr<Action> action);
    // void setWrapper(std::shared_ptr<Action> action, std::shared_ptr<Dialog> dialog);

    virtual void set() {}
    // virtual void sendData() {}
    virtual void act() {}

    void failed();

protected:
    std::shared_ptr<Dialog> dialog_;
    // std::shared_ptr<DialogWrapper> wrapper_;
};

class RegularAction : public Action
{
    // Q_OBJECT

public:
    RegularAction(std::shared_ptr<Dialog> dialog);
    ~RegularAction() = default;

    void set() override;
};

class CompanionAction : public RegularAction, public std::enable_shared_from_this<CompanionAction>
// class CompanionAction : public RegularAction, public std::enable_shared_from_this<Action>
{
    // Q_OBJECT

public:
    CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion);
    ~CompanionAction() = default;

    ChatActionType getType() const;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<Companion> getCompanion() const;

    void set() override;

    void updateCompanionObjectData();

// public slots:
    void act() override;
    // void failed() override;

private:
    ChatActionType type_;
    std::shared_ptr<CompanionData> data_;
    std::shared_ptr<Companion> companion_;
};

class GroupChatAction : public RegularAction
{
    // Q_OBJECT

public:
    GroupChatAction(ChatActionType type);
    ~GroupChatAction() = default;

private:
    ChatActionType type_;
    std::shared_ptr<GroupChatData> data_;
};

class PasswordAction : public RegularAction, public std::enable_shared_from_this<PasswordAction>
{
    // Q_OBJECT

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
    // Q_OBJECT

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
