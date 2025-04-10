#include "widgets_dialog.hpp"

CompanionDataDialog::CompanionDataDialog(
    ChatActionType actionType, QWidget* parentPtr, Companion* companionPtr) {
    setParent(parentPtr);

    setWindowTitle(
        getConstantMappingValue(
            "companionActionTypeStringRepresentation",
            &companionActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    nameLabelPtr_ = new QLabel("Name");
    nameEditPtr_ = new QLineEdit;

    ipAddressLabelPtr_ = new QLabel("IpAddress");
    ipAddressEditPtr_ = new QLineEdit;

    portLabelPtr_ = new QLabel("Port");
    portEditPtr_ = new QLineEdit;

    if(actionType_ == ChatActionType::UPDATE && companionPtr) {
        nameEditPtr_->setText(getQString(companionPtr->getName()));

        ipAddressEditPtr_->setText(
            getQString(companionPtr->getSocketInfoPtr()->getIpAddress()));

        portEditPtr_->setText(
            getQString(std::to_string(
                companionPtr->getSocketInfoPtr()->getClientPort())));
    }

    layoutPtr_->addRow(nameLabelPtr_, nameEditPtr_);
    layoutPtr_->addRow(ipAddressLabelPtr_, ipAddressEditPtr_);
    layoutPtr_->addRow(portLabelPtr_, portEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CompanionDataDialog::~CompanionDataDialog() {
    delete this->layoutPtr_;
    delete this->nameLabelPtr_;
    delete this->nameEditPtr_;
    delete this->ipAddressLabelPtr_;
    delete this->ipAddressEditPtr_;
    delete this->portLabelPtr_;
    delete this->portEditPtr_;
    delete this->buttonBoxPtr_;
}

void CompanionDataDialog::set() {
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);

    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::rejected,
        this, &QDialog::reject, Qt::QueuedConnection);
}

std::string CompanionDataDialog::getNameString() {
    return this->nameEditPtr_->text().toStdString();
}

std::string CompanionDataDialog::getIpAddressString() {
    auto ipAddressFromWidget = this->ipAddressEditPtr_->text().toStdString();  // TODO change
    QHostAddress hostAddress { getQString(ipAddressFromWidget) };

    return hostAddress.toString().toStdString();
}

std::string CompanionDataDialog::getPortString() {
    return this->portEditPtr_->text().toStdString();
}

GroupChatDataDialog::GroupChatDataDialog(
    ChatActionType actionType, QWidget* parentPtr) {

    setParent(parentPtr);

    setWindowTitle(
        getConstantMappingValue(
            "groupChatActionTypeStringRepresentation",
            &groupChatActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;




}

GroupChatDataDialog::~GroupChatDataDialog() {}

void GroupChatDataDialog::set() {}

CreatePasswordDialog::CreatePasswordDialog() {
    setWindowTitle(newPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    firstLabelPtr_ = new QLabel(newPasswordDialogFirstLabel);
    firstEditPtr_ = new QLineEdit;

    secondLabelPtr_ = new QLabel(newPasswordDialogSecondLabel);
    secondEditPtr_ = new QLineEdit;

    layoutPtr_->addRow(firstLabelPtr_, firstEditPtr_);
    layoutPtr_->addRow(secondLabelPtr_, secondEditPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

CreatePasswordDialog::~CreatePasswordDialog() {
    delete this->layoutPtr_;
    delete this->firstLabelPtr_;
    delete this->firstEditPtr_;
    delete this->secondLabelPtr_;
    delete this->secondEditPtr_;
    delete this->buttonBoxPtr_;
}

void CreatePasswordDialog::set() {
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::getFirstEditText() {
    return this->firstEditPtr_->text().toStdString();
}

std::string CreatePasswordDialog::getSecondEditText() {
    return this->secondEditPtr_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog() {
    setWindowTitle(getPasswordDialogTitle);

    setParent(getGraphicManagerPtr()->getMainWindowPtr());

    setModal(true);
    setWindowFlag(Qt::Window);

    layoutPtr_ = new QFormLayout;
    setLayout(layoutPtr_);

    labelPtr_ = new QLabel(getPasswordDialogLabel);
    editPtr_ = new QLineEdit;

    layoutPtr_->addRow(labelPtr_, editPtr_);

    buttonBoxPtr_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layoutPtr_->addWidget(buttonBoxPtr_);
}

GetPasswordDialog::~GetPasswordDialog() {
    delete this->layoutPtr_;
    delete this->labelPtr_;
    delete this->editPtr_;
    delete this->buttonBoxPtr_;
}

void GetPasswordDialog::set() {
    connect(
        this->buttonBoxPtr_, &QDialogButtonBox::accepted,
        this->actionPtr_, &Action::sendData, Qt::QueuedConnection);
}

std::string GetPasswordDialog::getEditText() {
    return this->editPtr_->text().toStdString();
}

ButtonInfo::ButtonInfo(
    const QString& buttonText, QDialogButtonBox::ButtonRole buttonRole,
    void (TextDialog::*functionPtr)()) :
    buttonText_(buttonText), buttonRole_(buttonRole), functionPtr_(functionPtr) {}

TextDialog::TextDialog(
    QWidget* parentPtr, DialogType dialogType, const std::string& text,
    std::vector<ButtonInfo>* buttonsInfoPtr) {
    if(parentPtr) {
        setParent(parentPtr);
    }
    else {
        setParent(getGraphicManagerPtr()->getMainWindowPtr());
    }

    setModal(true);
    setWindowFlag(Qt::Window);

    setWindowTitle(
        getConstantMappingValue(
            "dialogTypeStringRepresentation",
            &dialogTypeStringRepresentation,
            dialogType));

    layoutPtr_ = new QVBoxLayout;
    setLayout(layoutPtr_);

    textEditPtr_ = new QPlainTextEdit;
    textEditPtr_->setReadOnly(true);
    textEditPtr_->setPlainText(getQString(text));
    layoutPtr_->addWidget(textEditPtr_);

    // set button box
    buttonBoxPtr_ = new QDialogButtonBox;
    layoutPtr_->addWidget(buttonBoxPtr_);

    buttonsInfoPtr_ = buttonsInfoPtr;
}

TextDialog::~TextDialog() {
    delete this->layoutPtr_;
    delete this->textEditPtr_;
    delete this->buttonBoxPtr_;
    delete this->buttonsInfoPtr_;
}

void TextDialog::set() {
    for(auto& info : *this->buttonsInfoPtr_) {
        QPushButton* buttonPtr = this->buttonBoxPtr_->addButton(
            info.buttonText_, info.buttonRole_);

        if(info.buttonRole_ == QDialogButtonBox::AcceptRole) {
            connect(
                this->buttonBoxPtr_, &QDialogButtonBox::accepted,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else if(info.buttonRole_ == QDialogButtonBox::RejectRole) {
            connect(
                this->buttonBoxPtr_, &QDialogButtonBox::rejected,
                this, info.functionPtr_, Qt::QueuedConnection);
        }
        else {
            showErrorDialogAndLogError("Unmanaged button role");
        }
    }
}

void TextDialog::closeSelf() {
    this->close();
}

void TextDialog::closeSelfAndParentDialog() {
    this->close();

    auto parentPtr = this->parent();

    if(parentPtr) {
        dynamic_cast<QWidget*>(parentPtr)->close();
    }
}

void TextDialog::acceptAction() {
    this->close();
    this->actionPtr_->sendData();
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs() {
    getGraphicManagerPtr()->disableMainWindowBlurEffect();
    this->closeSelfAndParentDialog();
}

void TextDialog::reject() {
    QDialog::reject();
}

FileDialog::FileDialog(FileAction* actionPtr, const QString& windowTitle) {
    actionPtr_ = actionPtr;

    containsDialogPtr_ = true;
    fileDialogPtr_ = new QFileDialog;
    fileDialogPtr_->setFileMode(QFileDialog::AnyFile);
    fileDialogPtr_->setViewMode(QFileDialog::Detail);

    fileDialogPtr_->
        setDirectory(getQString(getManagerPtr()->getLastOpenedPath().string()));

    fileDialogPtr_->setWindowTitle(windowTitle);
}

FileDialog::~FileDialog() {
    delete this->fileDialogPtr_;
}

void FileDialog::set() {
    connect(
        this->fileDialogPtr_, &QFileDialog::accepted,
        this->actionPtr_, &Action::sendData,
        Qt::QueuedConnection);
}

void FileDialog::showDialog() {
    this->fileDialogPtr_->show();
}

QFileDialog* FileDialog::getFileDialogPtr() {
    return this->fileDialogPtr_;
}
