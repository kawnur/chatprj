#ifndef ACTION_HPP
#define ACTION_HPP

#include <QObject>

#include "companion.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

class Dialog;
class Companion;
class CompanionData;
class MainWindow;

class Action : public QObject
{
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

class CompanionAction : public Action
{
    Q_OBJECT

public:
    CompanionAction(CompanionActionType, MainWindow*, Companion*);
    ~CompanionAction();

    CompanionActionType getActionType();

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

    MainWindow* mainWindowPtr_;
    Companion* companionPtr_;
};

class PasswordAction : public Action
{
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

#endif // ACTION_HPP
