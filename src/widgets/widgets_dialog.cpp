#include "widgets_dialog.hpp"

#include <filesystem>

#include <QHostAddress>

#include "action.hpp"
#include "action_wrapper.hpp"
#include "companion.hpp"
#include "data.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"
#include "utils.hpp"
#include "widgets.hpp"

Dialog::Dialog()
    : dialogType_(DialogType::INFO), actionType_(ActionType::UNKNOWN), containsDialog_(false),
    dataMap_(ACTION_TITLE_MAP), infoMap_(INFO_STUB),
    defaultTitle_(getQString(ACTION_INFO_DIALOG_DEFAULT_HEADER)) {}

void Dialog::setTitleAsData()
{
    auto title = getMapValue(dataMap_, actionType_, defaultTitle_);
    setWindowTitle(title);
}

void Dialog::setTitleAsInfo()
{
    auto title = getMapValue(infoMap_, dialogType_, defaultTitle_);
    setWindowTitle(title);
}

void Dialog::setDialogType(DialogType type)
{
    dialogType_ = type;
    setTitleAsInfo();
}

CompanionDataDialog::CompanionDataDialog(
    ActionType type, QWidgetPtr parent, CompanionPtr companion)
    : Dialog()
{
    setTitleAsData();

    setParent(parent.get());
    setModal(true);
    setWindowFlag(Qt::Window);

    type_ = type;

    layout_ = std::make_unique<QFormLayout>();
    setLayout(layout_.get());

    nameLabel_ = std::make_unique<QLabel>("Name");
    nameEdit_ = std::make_unique<QLineEdit>();

    ipAddressLabel_ = std::make_unique<QLabel>("IpAddress");
    ipAddressEdit_ = std::make_unique<QLineEdit>();

    portLabel_ = std::make_unique<QLabel>("Port");
    portEdit_ = std::make_unique<QLineEdit>();

    if (type_ == ActionType::UPDATE_COMPANION && companion) {
        auto data = getManager()->getCompanionData(companion);

        if (data) {
            nameEdit_->setText(getQString(companion->name()));
            ipAddressEdit_->setText(getQString(data->ipAddress()));
            portEdit_->setText(getQString(data->clientPort()));
        }
    }

    layout_->addRow(nameLabel_.get(), nameEdit_.get());
    layout_->addRow(ipAddressLabel_.get(), ipAddressEdit_.get());
    layout_->addRow(portLabel_.get(), portEdit_.get());

    buttonBox_ = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout_->addWidget(buttonBox_.get());
}

std::string CompanionDataDialog::name()
{
    return nameEdit_->text().toStdString();
}

std::string CompanionDataDialog::ipAddress()
{
    // result format is db-compliant
    auto ipAddressFromWidget = ipAddressEdit_->text();
    QHostAddress hostAddress { ipAddressFromWidget };

    return hostAddress.toString().toStdString();
}

std::string CompanionDataDialog::port()
{
    return portEdit_->text().toStdString();
}

void CompanionDataDialog::set(ActionPtr action)
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action.get(), &Action::act, Qt::QueuedConnection);

    connect(
        buttonBox_.get(), &QDialogButtonBox::rejected,
        action.get(), &Action::endAct, Qt::QueuedConnection);

    // close button
    connect(
        this, &QDialog::rejected,
        action.get(), &Action::endAct, Qt::QueuedConnection);

}

CompanionDataPtr CompanionDataDialog::getCompanionData()
{
    return std::make_shared<CompanionData>(
        name(), ipAddress(), ""s, port());
}

GroupChatDataDialog::GroupChatDataDialog(ActionType type, QWidgetPtr parent)
{
    setTitleAsData();

    setParent(parent.get());
    setModal(true);
    setWindowFlag(Qt::Window);

    type_ = type;

    layout_ = std::make_unique<QVBoxLayout>();
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>(newGroupChatDialogLabel);

    list_ = std::make_unique<QListWidget>();
    layout_->addWidget(list_.get());
}

void GroupChatDataDialog::set(ActionPtr action) {}

CreatePasswordDialog::CreatePasswordDialog()
{
    actionType_ = ActionType::CREATE_PASSWORD;
    setTitleAsData();

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

void CreatePasswordDialog::set(ActionPtr action)
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action.get(), &Action::act, Qt::QueuedConnection);
}

std::string CreatePasswordDialog::firstEditText()
{
    return firstEdit_->text().toStdString();
}

std::string CreatePasswordDialog::secondEditText()
{
    return secondEdit_->text().toStdString();
}

GetPasswordDialog::GetPasswordDialog()
{
    actionType_ = ActionType::GET_PASSWORD;
    setTitleAsData();

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

void GetPasswordDialog::set(ActionPtr action)
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted,
        action.get(), &Action::act, Qt::QueuedConnection);

    // close button
    connect(
        this, &QDialog::rejected,
        action.get(), &Action::endAct, Qt::QueuedConnection);
}

std::string GetPasswordDialog::editText()
{
    return edit_->text().toStdString();
}

TextDialog::TextDialog(
    std::shared_ptr<Dialog> parent, DialogType type, const std::string &text,
    std::initializer_list<ButtonInfo> buttonsInfo)
{
    // title
    dialogType_ = type;
    infoMap_ = DIALOG_TYPE_STRING_REPR_MAP;
    defaultTitle_ = getQString(INFO_DIALOG_DEFAULT_HEADER);
    setTitleAsInfo();

    // parent
    if (parent)
        setParent(parent.get());
    else
        setParent(getGraphicManager()->getMainWindow().get());

    // modality
    setModal(true);

    // flags
    setWindowFlag(Qt::Window);
    // setWindowFlags(windowFlags() & Qt::CustomizeWindowHint & ~Qt::WindowCloseButtonHint);
    // setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    // set fields
    layout_ = std::make_unique<QVBoxLayout>();
    setLayout(layout_.get());

    edit_ = std::make_unique<QPlainTextEdit>();
    edit_->setReadOnly(true);
    edit_->setPlainText(getQString(text));
    layout_->addWidget(edit_.get());

    // set button box
    buttonBox_ = std::make_unique<ButtonBox>(buttonsInfo);
    layout_->addWidget(buttonBox_.get());
}

TextDialog::~TextDialog() { logArgsInfo(Q_FUNC_INFO); }

void TextDialog::set(ActionPtr action)
{
    connect(this, &QDialog::rejected, action.get(), &Action::endAct, Qt::QueuedConnection);
}

void TextDialog::closeSelf()
{
    close();
}

void TextDialog::closeSelfAndParent()
{
    close();

    auto parentWidget = parent();

    if (!parentWidget)
        return;

    auto cast = qobject_cast<QWidget *>(parentWidget);

    if (cast && cast != getGraphicManager()->getMainWindow().get())
        cast->close();
}

void TextDialog::acceptAction()
{
    close();
}

void TextDialog::setText(const std::string &text)
{
    edit_->setPlainText(getQString(text));
}

void TextDialog::update(
    DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list)
{
    updateType(type);
    setText(text);
    updateButtonBox(list);
}

void TextDialog::updateType(DialogType type)
{
    if (dialogType_ != type) {
        dialogType_ = type;
        setTitleAsInfo();
    }
}

void TextDialog::updateButtonBox(std::initializer_list<ButtonInfo> list)
{
    buttonBox_ = std::make_unique<ButtonBox>(list);
    layout_->addWidget(buttonBox_.get());
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs()
{
    getGraphicManager()->disableMainWindowBlurEffect();
    closeSelfAndParent();
}

void TextDialog::reject()
{
    QDialog::reject();
}

FileDialog::FileDialog(ActionType type)
{
    // title
    actionType_ = type;
    setTitleAsData();

    containsDialog_ = true;
    dialog_ = std::make_shared<QFileDialog>();
    dialog_->setFileMode(QFileDialog::AnyFile);
    dialog_->setViewMode(QFileDialog::Detail);
    dialog_->setDirectory(getQString(getManager()->getLastOpenedPath().string()));
    // dialog_->setWindowTitle(getQString(title));
}

FileDialog::~FileDialog() { logArgsInfo(Q_FUNC_INFO); }

void FileDialog::set(ActionPtr action)
{
    connect(
        dialog_.get(), &QFileDialog::accepted,
        action.get(), &Action::act, Qt::QueuedConnection);
}

void FileDialog::showDialog()
{
    dialog_->show();
}

std::shared_ptr<QFileDialog> FileDialog::getFileDialog()
{
    return dialog_;
}

QStringList FileDialog::selectedFiles()
{
    return dialog_->selectedFiles();
}
