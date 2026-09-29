#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "constants.hpp"
#include "manager.hpp"
#include "widgets_dialog.hpp"

class Action;
class Dialog;
class Companion;
class CompanionData;
class GroupChatData;

// class ActionWrapperBase
// {
// public:
//     ActionWrapperBase() = default;
//     virtual ~ActionWrapperBase() { logArgsInfo(Q_FUNC_INFO); }

//     virtual void act() {}
//     virtual void failed() {}
//     virtual void method1() {}
//     virtual std::shared_ptr<Action> getAction() { return nullptr; }
//     virtual std::shared_ptr<CompanionData> getCompanionData() { return nullptr; }
// };

// template<typename T, typename...Ts>
// class ActionWrapper
//     : public ActionWrapperBase, public std::enable_shared_from_this<ActionWrapper<T, Ts...>>
// {
// public:
//     ActionWrapper(Ts &&...params)
//     {
//         auto action = std::make_shared<T>(params...);
//         dialog_ = action->buildDialog();

//         auto cast = std::dynamic_pointer_cast<Action>(action);

//         if (!cast)
//             logArgsError("action cast error");

//         action_ = cast;
//     }

//     ~ActionWrapper() { logArgsInfo(Q_FUNC_INFO); }

//     std::shared_ptr<Action> getAction() { return action_; }
//     std::shared_ptr<Dialog> getDialog() { return dialog_; }

//     void set()
//     {
//         auto cast = std::dynamic_pointer_cast<ActionWrapperBase>(this->shared_from_this());

//         if (!cast)
//             logArgsError("action cast error");

//         action_->setWrapper(cast);

//         dialog_->setWrapper(cast);
//         dialog_->set();
//         dialog_->show();
//     }

//     std::shared_ptr<CompanionData> getCompanionData() override
//     {
//         return dialog_->getCompanionData();
//     }

//     void showDialog();

//     void act() override
//     {
//         action_->act();
//         getManager()->performCompanionAction(this->shared_from_this());
//     }

//     void failed() override { action_->failed(); }

//     void method1() override
//     {
//         action_->method1();
//         dialog_->method1();
//     }

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
    virtual ~Action();

    virtual std::shared_ptr<Dialog> buildDialog() { return nullptr; }
    // void setDialog(std::shared_ptr<Action> action);
    void setWrapper(std::shared_ptr<ActionWrapperBase> wrapper);

    virtual void set() {}
    // virtual void sendData() {}
    virtual void act() {}
    virtual void method1() {}

    virtual ChatActionType getType() { return ChatActionType::UNKNOWN; }

    void failed();

protected:
    // std::shared_ptr<Dialog> dialog_;
    std::shared_ptr<ActionWrapperBase> wrapper_;
};

class RegularAction : public Action
{
    // Q_OBJECT

public:
    RegularAction(std::shared_ptr<Dialog> dialog);
    virtual ~RegularAction();

    void set() override;
};

class CompanionAction : public RegularAction, public std::enable_shared_from_this<CompanionAction>
// class CompanionAction : public RegularAction, public std::enable_shared_from_this<Action>
{
    // Q_OBJECT

public:
    CompanionAction(ChatActionType type, std::shared_ptr<Companion> companion);
    ~CompanionAction();

    ChatActionType getType() override;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<Companion> getCompanion() const;

    std::shared_ptr<Dialog> buildDialog() override;
    void set() override;

    void updateCompanionObjectData();

// public slots:
    void act() override;
    // void failed() override;
    void method1() override;

private:
    ChatActionType type_;
    std::shared_ptr<CompanionData> data_;
    std::shared_ptr<Companion> companion_;
};

// class GroupChatAction : public RegularAction
// {
//     // Q_OBJECT

// public:
//     GroupChatAction(ChatActionType type);
//     ~GroupChatAction() = default;

// private:
//     ChatActionType type_;
//     std::shared_ptr<GroupChatData> data_;
// };

// class PasswordAction : public RegularAction, public std::enable_shared_from_this<PasswordAction>
// {
//     // Q_OBJECT

// public:
//     PasswordAction(PasswordActionType type);
//     ~PasswordAction();

//     std::string getPassword();
//     void act() override;

// private:
//     PasswordActionType type_;
//     std::string password_;
// };

// class FileAction : public Action, public std::enable_shared_from_this<FileAction>
// {
//     // Q_OBJECT

// public:
//     FileAction(
//         FileActionType type, const std::string &networkId,
//         std::shared_ptr<Companion> companion);

//     ~FileAction() = default;

//     std::shared_ptr<Companion> getCompanion() const;
//     std::filesystem::path getPath() const;
//     void set() override;

//     void act() override;
//     void defineFilePath();

// private:
//     FileActionType type_;
//     std::filesystem::path filePath_;
//     std::shared_ptr<Companion> companion_;
//     std::string networkId_;
// };

#endif // ACTION_HPP
