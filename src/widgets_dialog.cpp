#include "widgets_dialog.hpp"

CompanionDataDialog::CompanionDataDialog(
    ChatActionType actionType, std::shared_ptr<QWidget> parent,
    std::shared_ptr<Companion> companion)
{
    setParent(parent);

    setWindowTitle(
        getConstantMappingValue(
            "companionActionTypeStringRepresentation",
            &companionActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layout_ = new QFormLayout;
    setLayout(layout_);

    nameLabel_ = new QLabel("Name");
    nameEdit_ = new QLineEdit;

    ipAddressLabel_ = new QLabel("IpAddress");
    ipAddressEdit_ = new QLineEdit;

    portLabel_ = new QLabel("Port");
    portEdit_ = new QLineEdit;

    if (actionType_ == ChatActionType::UPDATE && companion) {
        nameEdit_->setText(getQString(companion->getName()));
        ipAddressEdit_->setText(getQString(companion->getSocketInfo()->getIpAddress()));
        portEdit_->setText(getQString(std::to_string(companion->getSocketInfo()->getClientPort())));
    }

    layout_->addRow(nameLabel_, nameEdit_);
    layout_->addRow(ipAddressLabel_, ipAddressEdit_);
    layout_->addRow(portLabel_, portEdit_);

    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout_->addWidget(buttonBox_);
}

void CompanionDataDialog::set()
{
    connect(
        buttonBox_, &QDialogButtonBox::accepted,
        action_, &Action::sendData, Qt::QueuedConnection);

    connect(
        buttonBox_, &QDialogButtonBox::rejected,
        this, &QDialog::reject, Qt::QueuedConnection);
}

std::string CompanionDataDialog::getNameString()
{
    return nameEdit_->text().toStdString();
}

std::string CompanionDataDialog::getIpAddressString()
{
    auto ipAddressFromWidget = ipAddressEdit_->text().toStdString();  // TODO change
    QHostAddress hostAddress { getQString(ipAddressFromWidget) };

    return hostAddress.toString().toStdString();
}

std::string CompanionDataDialog::getPortString()
{
    return portEdit_->text().toStdString();
}

GroupChatDataDialog::GroupChatDataDialog(ChatActionType actionType, std::shared_ptr<QWidget> parent)
{
    setParent(parent);

    setWindowTitle(
        getConstantMappingValue(
            "groupChatActionTypeStringRepresentation",
            &groupChatActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layout_ = new QVBoxLayout;
    setLayout(layout_);

    label_ = new QLabel(newGroupChatDialogLabel);

    list_ = new QListWidget;
    layout_->addWidget(list_);
}

void GroupChatDataDialog::set() {}

CreatePasswordDialog::CreatePasswordDialog() {
    setWindowTitle(newPasswordDialogTitle);

    setParent(getGraphicManager()->getMainWindow());

    setModal(true);
    setWindowFlag(Qt::Window);

    layout_ = new QFormLayout;
    setLayout(layout_);

    firstLabel_ = new QLabel(newPasswordDialogFirstLabel);
    firstEdit_ = new QLineEdit;

    secondLabel_ = new QLabel(newPasswordDialogSecondLabel);
    secondEdit_ = new QLineEdit;

    layout_->addRow(firstLabel_, firstEdit_);
    layout_->addRow(secondLabel_, secondEdit_);

    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout_->addWidget(buttonBox_);
}

void CreatePasswordDialog::set() {
    connect(
        buttonBox_, &QDialogButtonBox::accepted,
        action_, &Action::sendData, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::getFirstEditText() {
    return firstEdit_->text().toStdString();
}

std::string CreatePasswordDialog::getSecondEditText() {
    return secondEdit_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog() {
    setWindowTitle(getPasswordDialogTitle);

    setParent(getGraphicManager()->getMainWindow());

    setModal(true);
    setWindowFlag(Qt::Window);

    layout_ = new QFormLayout;
    setLayout(layout_);

    label_ = new QLabel(getPasswordDialogLabel);
    edit_ = new QLineEdit;

    layout_->addRow(label_, edit_);

    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout_->addWidget(buttonBox_);
}

void GetPasswordDialog::set() {
    connect(
        buttonBox_, &QDialogButtonBox::accepted,
        action_, &Action::sendData, Qt::QueuedConnection);
}

std::string GetPasswordDialog::getEditText() {
    return edit_->text().toStdString();
}

ButtonInfo::ButtonInfo(
    const QString& buttonText, QDialogButtonBox::ButtonRole buttonRole,
    void (TextDialog::*function)()) :
    buttonText_(buttonText), buttonRole_(buttonRole), function_(function) {}

TextDialog::TextDialog(
    std::shared_ptr<QWidget> parent, DialogType dialogType, const std::string& text,
    std::shared_ptr<std::vector<ButtonInfo>> buttonsInfo)
{
    if (parent) {
        setParent(parent);
    }
    else {
        setParent(getGraphicManager()->getMainWindow());
    }

    setModal(true);
    setWindowFlag(Qt::Window);

    setWindowTitle(
        getConstantMappingValue(
            "dialogTypeStringRepresentation",
            &dialogTypeStringRepresentation,
            dialogType));

    layout_ = new QVBoxLayout;
    setLayout(layout_);

    textEdit_ = new QPlainTextEdit;
    textEdit_->setReadOnly(true);
    textEdit_->setPlainText(getQString(text));
    layout_->addWidget(textEdit_);

    // set button box
    buttonBox_ = new QDialogButtonBox;
    layout_->addWidget(buttonBox_);

    buttonsInfo_ = buttonsInfo;
}

void TextDialog::set() {
    // for (auto& info : *buttonsInfo_) {
    //     std::shared_ptr<QPushButton> button = buttonBox_->addButton(
    //         info.buttonText_, info.buttonRole_);

    //     // TODO create mapping and select signal by role
    //     if (info.buttonRole_ == QDialogButtonBox::AcceptRole) {
    //         connect(
    //             buttonBox_, &QDialogButtonBox::accepted,
    //             this, info.function_, Qt::QueuedConnection);
    //     }
    //     else if (info.buttonRole_ == QDialogButtonBox::RejectRole) {
    //         connect(
    //             buttonBox_, &QDialogButtonBox::rejected,
    //             this, info.function_, Qt::QueuedConnection);
    //     }
    //     else {
    //         showErrorDialogAndLogError("Unmanaged button role");
    //     }
    // }
}

void TextDialog::closeSelf() {
    close();
}

void TextDialog::closeSelfAndParentDialog() {
    close();

    auto parent = parent();

    if (!parent)
        return;

    auto cast = dynamic_cast<QWidget *>(parent);

    if (cast)
        cast->close();
}

void TextDialog::acceptAction() {
    close();
    action_->sendData();
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs() {
    getGraphicManager()->disableMainWindowBlurEffect();
    closeSelfAndParentDialog();
}

void TextDialog::reject() {
    QDialog::reject();
}

FileDialog::FileDialog(std::shared_ptr<FileAction> action, const QString& windowTitle) {
    action_ = action;

    containsDialog_ = true;
    fileDialog_ = new QFileDialog;
    fileDialog_->setFileMode(QFileDialog::AnyFile);
    fileDialog_->setViewMode(QFileDialog::Detail);

    fileDialog_->
        setDirectory(getQString(getManager()->getLastOpenedPath().string()));

    fileDialog_->setWindowTitle(windowTitle);
}

void FileDialog::set() {
    connect(
        fileDialog_, &QFileDialog::accepted,
        action_, &Action::sendData, Qt::QueuedConnection);
}

void FileDialog::showDialog() {
    fileDialog_->show();
}

std::shared_ptr<QFileDialog> FileDialog::getFileDialog() {
    return fileDialog_;
}
