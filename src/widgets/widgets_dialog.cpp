#include "widgets_dialog.hpp"

#include <QHostAddress>

#include "action.hpp"
#include "companion.hpp"
#include "data.hpp"
#include "graphic_manager.hpp"
#include "logging.hpp"
#include "mainwindow.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"
#include "widgets.hpp"

Dialog::Dialog(const std::string &title)
    : dialogType_(DialogType::INFO), defaultTitle_(getQString(ACTION_INFO_DIALOG_DEFAULT_HEADER))
{
    setWindowTitle(getQString(title));
}

void Dialog::setDialogType(DialogType type)
{
    dialogType_ = type;
}

FormLine::FormLine(const QString &name)
{
    label_ = std::make_shared<QLabel>(name);
    edit_ = std::make_shared<QLineEdit>();
}

std::shared_ptr<QLabel> FormLine::label() const
{
    return label_;
}

std::shared_ptr<QLineEdit> FormLine::edit() const
{
    return edit_;
}

QString FormLine::getLabelText()
{
    return label_->text();
}

QString FormLine::getEditText()
{
    return edit_->text();
}

void FormLine::setEditText(const QString &text)
{
    edit_->setText(text);
}

FormLayout::FormLayout() : layout_(new QFormLayout), map_() {}

std::shared_ptr<QFormLayout> FormLayout::getLayout() const
{
    return layout_;
}

QString FormLayout::getDataByType(DataType type)
{
    auto line = getMapValue(map_, type, nullptr);

    return (line) ? line->getEditText() : "";
}

std::shared_ptr<DataWrapper> FormLayout::getDataWrapper()
{
    auto data = std::make_shared<DataWrapper>();

    for (const auto &pair : map_)
        data->set(pair.first, getString(pair.second->getEditText()));

    return data;
}

void FormLayout::fillForm(std::initializer_list<DataType> list)
{
    for (const auto &item : list) {
        auto line = std::make_shared<FormLine>(getDataTypeStringRepr(item));
        layout_->addRow(line->label().get(), line->edit().get());

        map_[item] = line;
    }
}

void FormLayout::setFormData(CompanionDataPtr data)
{
    for (auto pair : map_)
        pair.second->setEditText(getQString(data->getData(pair.first)));
}

DataDialog::DataDialog(const std::string &title, const std::initializer_list<DataType> &lines)
    : Dialog(title), FormLayout()
{
    // set properties
    setParent(getGraphicManager()->getMainWindow().get());
    setModal(true);
    setWindowFlag(Qt::Window);

    auto layout = getLayout();
    setLayout(layout.get());

    // fill form
    fillForm(lines);

    // buttons
    buttonBox_ = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox_.get());
}

void DataDialog::set(ActionPtr action)
{
    connect(
        buttonBox_.get(), &QDialogButtonBox::accepted, action.get(), &Action::perform,
        Qt::QueuedConnection);

    connect(
        buttonBox_.get(), &QDialogButtonBox::rejected, action.get(), &Action::endAct,
        Qt::QueuedConnection);

    // close button
    connect(this, &QDialog::rejected, action.get(), &Action::endAct, Qt::QueuedConnection);
}

std::shared_ptr<DataWrapper> DataDialog::getData()
{
    return getDataWrapper();
}

CompanionDataDialog::CompanionDataDialog(CompanionPtr companion, const std::string &title)
    : DataDialog(title, { DataType::NAME, DataType::IP_ADDRESS, DataType::CLIENT_PORT }) {}

UpdateCompanionDataDialog::UpdateCompanionDataDialog(
    CompanionPtr companion, const std::string &title)
    : CompanionDataDialog(companion, title)
{
    // set form values
    if (!companion)
        return;

    auto data = getGraphicManager()->getCompanionData(companion);

    if (data)
        setFormData(data);
}

CompanionDataPtr CompanionDataDialog::getCompanionData()
{
    auto name = getDataByType(DataType::NAME);

    // result format is db-compliant
    auto ipAddressFromWidget = getDataByType(DataType::IP_ADDRESS);
    QHostAddress hostAddress { ipAddressFromWidget };
    auto ipAddress = getString(hostAddress);

    auto port = getDataByType(DataType::CLIENT_PORT);

    return std::make_shared<CompanionData>(name, ipAddress, ""s, port);
}

GroupChatDataDialog::GroupChatDataDialog(const std::string &title) : Dialog(title)
{
    setParent(getGraphicManager()->getMainWindow().get());
    setModal(true);
    setWindowFlag(Qt::Window);

    layout_ = std::make_unique<QVBoxLayout>();
    setLayout(layout_.get());

    label_ = std::make_unique<QLabel>(newGroupChatDialogLabel);

    list_ = std::make_unique<QListWidget>();
    layout_->addWidget(list_.get());
}

void GroupChatDataDialog::set(ActionPtr action) {}

TextDialog::TextDialog(
    std::shared_ptr<Dialog> parent, DialogType type, const std::string &title,
    const std::string &text, std::initializer_list<ButtonInfo> buttonsInfo)
    : Dialog(title)
{
    // title
    dialogType_ = type;
    defaultTitle_ = getQString(INFO_DIALOG_DEFAULT_HEADER);
    setWindowTitle(getQString(title));

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
    }
}

void TextDialog::updateButtonBox(std::initializer_list<ButtonInfo> list)
{
    buttonBox_ = std::make_unique<ButtonBox>(list);
    layout_->addWidget(buttonBox_.get());
}

void TextDialog::unsetMainWindowBlurAndCloseDialogs()
{
    getGraphicManager()->disableBlur();
    closeSelfAndParent();
}

void TextDialog::reject()
{
    QDialog::reject();
}

FileDialog::FileDialog(const std::string &title) : Dialog(title)
{
    // containsDialog_ = true;
    dialog_ = std::make_shared<QFileDialog>();
    dialog_->setFileMode(QFileDialog::AnyFile);
    dialog_->setViewMode(QFileDialog::Detail);
    dialog_->setDirectory(getQString(getGraphicManager()->getLastOpenedPath()));
}

FileDialog::~FileDialog() { logArgsInfo(Q_FUNC_INFO); }

void FileDialog::set(ActionPtr action)
{
    connect(
        dialog_.get(), &QFileDialog::accepted,
        action.get(), &Action::perform, Qt::QueuedConnection);
}

void FileDialog::showDialog()
{
    dialog_->show();
}

// std::shared_ptr<QFileDialog> FileDialog::getFileDialog()
// {
//     return dialog_;
// }

QStringList FileDialog::selectedFiles() const
{
    return dialog_->selectedFiles();
}

QString FileDialog::getSaveFileName(const QString &param, const QString &path)
{
    auto cast = dynamic_pointer_cast<QWidget>(getGraphicManager()->getMainWindow());

    if (!cast)
        return "";

    return dialog_->getSaveFileName(cast.get(), param, path);
}
