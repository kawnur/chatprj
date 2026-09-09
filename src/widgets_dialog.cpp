#include "widgets_dialog.hpp"

CompanionDataDialog::CompanionDataDialog(
    ChatActionType actionType, std::shared_ptr<QWidget> parent,
    std::shared_ptr<Companion> companion)
{
    setParent(parent.get());

    setWindowTitle(
        getConstantMappingValue(
            "companionActionTypeStringRepresentation",
            &companionActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layout_ = std::make_unique<QFormLayout>();
    setLayout(layout_.get());

    nameLabel_ = std::make_unique<QLabel>("Name");
    nameEdit_ = std::make_unique<QLineEdit>();

    ipAddressLabel_ = std::make_unique<QLabel>("IpAddress");
    ipAddressEdit_ = std::make_unique<QLineEdit>();

    portLabel_ = std::make_unique<QLabel>("Port");
    portEdit_ = std::make_unique<QLineEdit>();

    if (actionType_ == ChatActionType::UPDATE && companion) {
        nameEdit_->setText(getQString(companion->getName()));
        ipAddressEdit_->setText(getQString(companion->getSocketInfo()->getIpAddress()));
        portEdit_->setText(getQString(std::to_string(companion->getSocketInfo()->getClientPort())));
    }

    layout_->addRow(nameLabel_.get(), nameEdit_.get());
    layout_->addRow(ipAddressLabel_.get(), ipAddressEdit_.get());
    layout_->addRow(portLabel_.get(), portEdit_.get());

    buttonBox_ = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout_->addWidget(buttonBox_.get());
}

void CompanionDataDialog::set()
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action_.get(), &Action::sendData, Qt::QueuedConnection);

    connect(
        buttonBox_.get(), &QDialogButtonBox::rejected,
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
    setParent(parent.get());

    setWindowTitle(
        getConstantMappingValue(
            "groupChatActionTypeStringRepresentation",
            &groupChatActionTypeStringRepresentation,
            actionType));

    setModal(true);
    setWindowFlag(Qt::Window);

    actionType_ = actionType;

    layout_ = std::make_unique<QVBoxLayout>();
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>(newGroupChatDialogLabel);

    list_ = std::make_unique<QListWidget>();
    layout_->addWidget(list_.get());
}

void GroupChatDataDialog::set() {}

CreatePasswordDialog::CreatePasswordDialog()
{
    setWindowTitle(newPasswordDialogTitle);

    setParent(getGraphicManager()->getMainWindow().get());

    setModal(true);
    setWindowFlag(Qt::Window);

    layout_ = std::make_unique<QFormLayout>();
    setLayout(layout_.get());

    firstLabel_ = std::make_unique<QLabel>(newPasswordDialogFirstLabel);
    firstEdit_ = std::make_unique<QLineEdit>();

    secondLabel_ = std::make_unique<QLabel>(newPasswordDialogSecondLabel);
    secondEdit_ = std::make_unique<QLineEdit>();

    layout_->addRow(firstLabel_.get(), firstEdit_.get());
    layout_->addRow(secondLabel_.get(), secondEdit_.get());

    buttonBox_ = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok);
    layout_->addWidget(buttonBox_.get());
}

void CreatePasswordDialog::set()
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action_.get(), &Action::sendData, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::getFirstEditText()
{
    return firstEdit_->text().toStdString();
}

std::string CreatePasswordDialog::getSecondEditText()
{
    return secondEdit_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog()
{
    setWindowTitle(getPasswordDialogTitle);

    setParent(getGraphicManager()->getMainWindow().get());

    setModal(true);
    setWindowFlag(Qt::Window);

    layout_ = std::make_unique<QFormLayout>();
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>(getPasswordDialogLabel);
    edit_ = std::make_unique<QLineEdit>();

    layout_->addRow(label_.get(), edit_.get());

    buttonBox_ = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok);
    layout_->addWidget(buttonBox_.get());
}

void GetPasswordDialog::set()
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action_.get(), &Action::sendData, Qt::QueuedConnection);
}

std::string GetPasswordDialog::getEditText()
{
    return edit_->text().toStdString();
}

// ButtonInfo::ButtonInfo(
//     const QString& text, QDialogButtonBox::ButtonRole role, void (TextDialog::*function)())
//     : text_(text), role_(role), function_(function) {}

ButtonInfo::ButtonInfo(
    const QString &text, QDialogButtonBox::ButtonRole role, std::function<void(TextDialog &)> function)
    : text_(text), role_(role), function_(function) {}

QString ButtonInfo::getText()
{
    return text_;
}

std::function<void(TextDialog &)> ButtonInfo::getFunction()
{
    return function_;
}

TextDialog::TextDialog(
    std::shared_ptr<QWidget> parent, DialogType dialogType, const std::string& text,
    std::shared_ptr<std::vector<ButtonInfo>> buttonsInfo)
{
    if (parent)
        setParent(parent.get());
    else
        setParent(getGraphicManager()->getMainWindow().get());

    setModal(true);
    setWindowFlag(Qt::Window);

    setWindowTitle(
        getConstantMappingValue(
            "dialogTypeStringRepresentation",
            &dialogTypeStringRepresentation,
            dialogType));

    layout_ = std::make_unique<QVBoxLayout>();
    setLayout(layout_.get());

    textEdit_ = std::make_unique<QPlainTextEdit>();
    textEdit_->setReadOnly(true);
    textEdit_->setPlainText(getQString(text));
    layout_->addWidget(textEdit_.get());

    // set button box
    buttonBox_ = std::make_unique<QDialogButtonBox>();
    layout_->addWidget(buttonBox_.get());

    buttonsInfo_ = buttonsInfo;
}

void TextDialog::set()
{
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

void TextDialog::closeSelf()
{
    close();
}

void TextDialog::closeSelfAndParentDialog()
{
    close();

    auto parentWidget = parent();

    if (!parentWidget)
        return;

    auto cast = qobject_cast<QWidget *>(parentWidget);

    if (cast)
        cast->close();
}

void TextDialog::acceptAction()
{
    close();
    action_->sendData();
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs()
{
    getGraphicManager()->disableMainWindowBlurEffect();
    closeSelfAndParentDialog();
}

void TextDialog::reject()
{
    QDialog::reject();
}

FileDialog::FileDialog(std::shared_ptr<FileAction> action, const QString& windowTitle)
{
    action_ = action;
    containsDialog_ = true;
    fileDialog_ = std::make_shared<QFileDialog>();
    fileDialog_->setFileMode(QFileDialog::AnyFile);
    fileDialog_->setViewMode(QFileDialog::Detail);
    fileDialog_->setDirectory(getQString(getManager()->getLastOpenedPath().string()));
    fileDialog_->setWindowTitle(windowTitle);
}

void FileDialog::set()
{
    connect(
        fileDialog_.get(), &QFileDialog::accepted,
        action_.get(), &Action::sendData, Qt::QueuedConnection);
}

void FileDialog::showDialog()
{
    fileDialog_->show();
}

std::shared_ptr<QFileDialog> FileDialog::getFileDialog()
{
    return fileDialog_;
}
