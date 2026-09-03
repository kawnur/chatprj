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

class Action : public QObject {

    Q_OBJECT

public:
    Action(std::shared_ptr<Dialog> dialogPtr) : dialogPtr_(dialogPtr) {}
    ~Action() = default;

    void set();
    std::shared_ptr<Dialog> getDialogPtr();

    virtual void sendData() {}

protected:
    std::shared_ptr<Dialog> dialogPtr_;
};

class CompanionAction : public Action {

    Q_OBJECT

public:
    CompanionAction(ChatActionType, std::shared_ptr<Companion>);
    ~CompanionAction();

    ChatActionType getActionType() const;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    std::shared_ptr<Companion> getCompanionPtr() const;
    void updateCompanionObjectData();

public slots:
    void sendData() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<CompanionData> dataPtr_;
    std::shared_ptr<Companion> companionPtr_;
};

class GroupChatAction : public Action {

    Q_OBJECT

public:
    GroupChatAction(ChatActionType);
    ~GroupChatAction();

public slots:
    void sendData() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<GroupChatData> dataPtr_;
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
    std::shared_ptr<string> passwordPtr_;
};

class FileAction : public Action {

    Q_OBJECT

public:
    FileAction(FileActionType, const std::string&, std::shared_ptr<Companion>);
    ~FileAction() = default;

    FileActionType getType() const;
    std::shared_ptr<Companion> getCompanionPtr() const;
    std::filesystem::path getPath() const;
    void sendData() override;
    void defineFilePath();

private:
    FileActionType actionType_;
    std::filesystem::path filePath_;
    std::shared_ptr<Companion> companionPtr_;
    std::string networkId_;
};

#endif // ACTION_HPP
