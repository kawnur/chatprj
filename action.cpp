#include "action.hpp"

void Action::set()
{
    this->dialogPtr_->setAction(this);
    this->dialogPtr_->set();
    this->dialogPtr_->show();
}

Dialog* Action::getDialogPtr()
{
    return this->dialogPtr_;
}

CompanionAction::CompanionAction(
    CompanionActionType actionType, MainWindow* mainWindowPtr, Companion* companionPtr) :
    actionType_(actionType), mainWindowPtr_(mainWindowPtr), companionPtr_(companionPtr),
    dataPtr_(nullptr), Action(nullptr)
{
    switch(actionType)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
        dialogPtr_ = new CompanionDataDialog(actionType_, mainWindowPtr_, companionPtr_);
        break;

    case CompanionActionType::DELETE:
        dialogPtr_ = new TextDialog(
            nullptr,
            DialogType::WARNING,
            deleteCompanionDialogText,
            new std::vector<ButtonInfo> {
                                        ButtonInfo(
                                            cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
                                        ButtonInfo(
                                            deleteCompanionButtonText, QDialogButtonBox::AcceptRole,
                                            &TextDialog::acceptAction) });
        break;

    case CompanionActionType::CLEAR_HISTORY:
        dialogPtr_ = new TextDialog(
            nullptr,
            DialogType::WARNING,
            clearCompanionHistoryDialogText,
            new std::vector<ButtonInfo> {
                                        ButtonInfo(
                                            cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
                                        ButtonInfo(
                                            clearHistoryButtonText, QDialogButtonBox::AcceptRole,
                                            &TextDialog::acceptAction) });
        break;
    }
}

CompanionAction::~CompanionAction()
{
    delete this->dataPtr_;
}

CompanionActionType CompanionAction::getActionType()
{
    return this->actionType_;
}

std::string CompanionAction::getName() const
{
    return this->dataPtr_->getName();
}

std::string CompanionAction::getIpAddress() const
{
    return this->dataPtr_->getIpAddress();
}

std::string CompanionAction::getServerPort() const
{
    return this->dataPtr_->getServerPort();
}

std::string CompanionAction::getClientPort() const
{
    return this->dataPtr_->getClientPort();
}

int CompanionAction::getCompanionId() const
{
    return this->companionPtr_->getId();
}

Companion* CompanionAction::getCompanionPtr() const
{
    return this->companionPtr_;
}

void CompanionAction::sendData()
{
    std::string name;
    std::string ipAddress;
    std::string serverPort { "" };
    std::string clientPort;

    switch(this->actionType_)
    {
    case CompanionActionType::CREATE:
    case CompanionActionType::UPDATE:
    {
        CompanionDataDialog* dataDialogPtr =
            dynamic_cast<CompanionDataDialog*>(this->dialogPtr_);

        name = dataDialogPtr->getNameString();
        ipAddress = dataDialogPtr->getIpAddressString();
        clientPort = dataDialogPtr->getPortString();
    }
    break;

    case CompanionActionType::DELETE:
    case CompanionActionType::CLEAR_HISTORY:
    {
        name = this->companionPtr_->getName();
        ipAddress = this->companionPtr_->getIpAddress();
        clientPort = std::to_string(this->companionPtr_->getClientPort());
    }
    break;

    }

    logArgs("name:", name, "ipAddress:", ipAddress, "clientPort:", clientPort);

    this->dataPtr_ = new CompanionData(name, ipAddress, serverPort, clientPort);

    getGraphicManagerPtr()->sendCompanionDataToManager(this);
}

void CompanionAction::updateCompanionObjectData()
{
    this->companionPtr_->updateData(this->dataPtr_);
}

PasswordAction::PasswordAction(PasswordActionType actionType) : Action(nullptr)
{
    actionType_ = actionType;

    switch(actionType)
    {
    case PasswordActionType::CREATE:
        dialogPtr_ = new CreatePasswordDialog;
        break;

    case PasswordActionType::GET:
        dialogPtr_ = new GetPasswordDialog;
        break;
    }
}

PasswordAction::~PasswordAction()
{
    if(this->actionType_ == PasswordActionType::GET)
    {
        this->dialogPtr_->close();
    }
}

std::string PasswordAction::getPassword()
{
    return *this->passwordPtr_;
}

void PasswordAction::sendData()
{
    switch(this->actionType_)
    {
    case PasswordActionType::CREATE:
    {
        CreatePasswordDialog* passwordDialogPtr =
            dynamic_cast<CreatePasswordDialog*>(this->dialogPtr_);

        auto text1 = passwordDialogPtr->getFirstEditText();
        auto text2 = passwordDialogPtr->getSecondEditText();

        if(text1 == text2)
        {
            if(text1.size() == 0)
            {
                showErrorDialogAndLogError(this->getDialogPtr(), "Empty password is invalid");
                return;
            }

            this->passwordPtr_ = &text1;
            getGraphicManagerPtr()->sendNewPasswordDataToManager(this);
        }
        else
        {
            showErrorDialogAndLogError(this->getDialogPtr(), "Entered passwords are not equal");
        }
    }

    break;

    case PasswordActionType::GET:
    {
        GetPasswordDialog* passwordDialogPtr =
            dynamic_cast<GetPasswordDialog*>(this->dialogPtr_);

        auto text = passwordDialogPtr->getEditText();

        if(text.size() == 0)
        {
            showErrorDialogAndLogError(this->getDialogPtr(), "Empty password is invalid");
            return;
        }

        this->passwordPtr_ = &text;
        getGraphicManagerPtr()->sendExistingPasswordDataToManager(this);
    }

    break;
    }
}
