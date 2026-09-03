#ifndef ACTION_HPP
#define ACTION_HPP

#include <memory>
#include <filesystem>

#include <QObject>

#include "data.hpp"
#include "companion.hpp"
#include "constants.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

class Dialog;
class Companion;
class CompanionData;
class GroupChatData;
class MainWindow;

class Action : public QObject, public std::enable_shared_from_this<Action> {

    Q_OBJECT

public:
    Action(std::shared_ptr<Dialog> dialog) : dialog_(dialog) {}
    ~Action() = default;

    void set();
    std::shared_ptr<Dialog> getDialog();

    virtual void sendData() {}

protected:
    std::shared_ptr<Dialog> dialog_;
};

class CompanionAction : public Action {

    Q_OBJECT

public:
    CompanionAction(ChatActionType, std::shared_ptr<Companion>);
    ~CompanionAction() = default;

    ChatActionType getActionType() const;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<Companion> getCompanion() const;
    void updateCompanionObjectData();

public slots:
    void sendData() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<CompanionData> data_;
    std::shared_ptr<Companion> companion_;
};

class GroupChatAction : public Action {

    Q_OBJECT

public:
    GroupChatAction(ChatActionType);
    ~GroupChatAction() = default;

public slots:
    void sendData() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<GroupChatData> data_;
};

class PasswordAction : public Action {

    Q_OBJECT

public:
    PasswordAction(PasswordActionType);
    ~PasswordAction();

    std::string getPassword();
    void sendData() override;

private:
    PasswordActionType actionType_;
    std::string password_;
};

class FileAction : public Action {

    Q_OBJECT

public:
    FileAction(FileActionType, const std::string&, std::shared_ptr<Companion>);
    ~FileAction() = default;

    FileActionType getType() const;
    std::shared_ptr<Companion> getCompanion() const;
    std::filesystem::path getPath() const;
    void sendData() override;
    void defineFilePath();

private:
    FileActionType actionType_;
    std::filesystem::path filePath_;
    std::shared_ptr<Companion> companion_;
    std::string networkId_;
};

#endif // ACTION_HPP
