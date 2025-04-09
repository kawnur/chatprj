#ifndef ACTION_HPP
#define ACTION_HPP

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
class MainWindow;

class Action : public QObject {

    Q_OBJECT

public:
    Action(Dialog* dialogPtr) : dialogPtr_(dialogPtr) {}
    ~Action() = default;

    void set();
    Dialog* getDialogPtr();

    virtual void sendData() {}

protected:
    Dialog* dialogPtr_;
};

class CompanionAction : public Action {

    Q_OBJECT

public:
    CompanionAction(CompanionActionType, MainWindow*, Companion*);
    ~CompanionAction();

    CompanionActionType getActionType() const;
    std::string getName() const;
    std::string getIpAddress() const;
    std::string getServerPort() const;
    std::string getClientPort() const;
    int getCompanionId() const;
    Companion* getCompanionPtr() const;
    void updateCompanionObjectData();

public slots:
    void sendData() override;

private:
    CompanionActionType actionType_;
    CompanionData* dataPtr_;
    MainWindow* mainWindowPtr_;  // TODO do we need it here?
    Companion* companionPtr_;
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
    const std::string* passwordPtr_;
};

class FileAction : public Action {

    Q_OBJECT

public:
    FileAction(FileActionType, const std::string&, Companion*);
    ~FileAction() = default;

    FileActionType getType() const;
    Companion* getCompanionPtr() const;
    std::filesystem::path getPath() const;
    void sendData() override;
    void defineFilePath();

private:
    FileActionType actionType_;
    std::filesystem::path filePath_;
    Companion* companionPtr_;
    std::string networkId_;
};

#endif // ACTION_HPP
