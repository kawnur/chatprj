#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QString>

#include "alias.hpp"
#include "data.hpp"
#include "constants.hpp"
#include "logging.hpp"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(ActionType actionType);
    virtual ~Dialog() { logArgsInfo(Q_FUNC_INFO); }

    virtual void set(ActionPtr action) {}
    virtual void showDialog() {}
    virtual std::shared_ptr<DataWrapper> getDataWrapper() { return nullptr; }
    virtual CompanionDataPtr getCompanionData() { return nullptr; }
    virtual std::string editText() { return ""s; }
    virtual QStringList selectedFiles() const { return QStringList(); }
    virtual QString getSaveFileName(const QString &param, const QString &path) { return ""; }

    void setDialogType(DialogType type);

public slots:
    // void actionSlot();

protected:
    void setTitleAsData();
    void setTitleAsInfo();

    DialogType dialogType_;
    ActionType actionType_;
    // bool containsDialog_;
    std::reference_wrapper<const DataDialogTitleMap> dataMap_;
    std::reference_wrapper<const InfoDialogTitleMap> infoMap_;
    QString defaultTitle_;
};

class FormLine
{
public:
    FormLine(const QString &name);
    ~FormLine() = default;

    std::shared_ptr<QLabel> label() const;
    std::shared_ptr<QLineEdit> edit() const;
    QString getLabelText();
    QString getEditText();

    void setEditText(const QString &text);

private:
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QLineEdit> edit_;
};

class FormLayout
{
public:
    FormLayout();
    ~FormLayout() = default;

    std::shared_ptr<QFormLayout> getLayout() const;
    QString getData(DataType type);
    std::shared_ptr<DataWrapper> getDataWrapper1();

    void fillForm(std::initializer_list<DataType> list);
    void setFormData(CompanionDataPtr data);

private:
    std::shared_ptr<QFormLayout> layout_;
    std::unordered_map<DataType, std::shared_ptr<FormLine>> map_;
};

class DataDialog : public Dialog, public FormLayout
{
    Q_OBJECT

public:
    DataDialog(ActionType type, const std::initializer_list<DataType> &lines);
    virtual ~DataDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;
    std::shared_ptr<DataWrapper> getDataWrapper() override;

private:
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class CompanionDataDialog : public DataDialog
{
    Q_OBJECT

public:
    CompanionDataDialog(ActionType type, CompanionPtr companion);
    ~CompanionDataDialog() { logArgsInfo(Q_FUNC_INFO); }

    CompanionDataPtr getCompanionData() override;
};

class GroupChatDataDialog : public Dialog
{
    Q_OBJECT

public:
    GroupChatDataDialog(ActionType type);
    ~GroupChatDataDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;

private:
    ActionType type_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QListWidget> list_;
};

class TextDialog : public Dialog
{
    Q_OBJECT

public:
    TextDialog(
        std::shared_ptr<Dialog> parent, DialogType type, const std::string &text,
        std::initializer_list<ButtonInfo> buttonsInfo);

    ~TextDialog();

    void set(ActionPtr action) override;
    void closeSelf();
    void closeSelfAndParent();
    void acceptAction();
    void setText(const std::string &text);

    void update(DialogType type, const std::string &text, std::initializer_list<ButtonInfo> list);
    void updateType(DialogType type);
    void updateButtonBox(std::initializer_list<ButtonInfo> list);

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    std::unique_ptr<QPlainTextEdit> edit_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<ButtonBox> buttonBox_;
};

class FileDialog : public Dialog
{
    Q_OBJECT

public:
    FileDialog(ActionType type);
    ~FileDialog();

    void set(ActionPtr action) override;
    void showDialog() override;

    // std::shared_ptr<QFileDialog> getFileDialog();
    QStringList selectedFiles() const override;

    QString getSaveFileName(const QString &param, const QString &path) override;

private:
    std::shared_ptr<QFileDialog> dialog_;
};

#endif // WIDGETS_DIALOG_HPP
