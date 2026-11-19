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
    Dialog(const std::string &title);
    virtual ~Dialog() { logArgsInfo(Q_FUNC_INFO); }

    virtual void set(ActionPtr action) {}
    virtual void showDialog() {}
    virtual std::shared_ptr<DataWrapper> getData() { return nullptr; }
    virtual CompanionDataPtr getCompanionData() { return nullptr; }
    virtual std::string editText() { return ""s; }
    virtual QStringList selectedFiles() const { return QStringList(); }
    virtual QString getSaveFileName(const QString &param, const QString &path) { return ""; }
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
    QString getDataByType(DataType type);
    std::shared_ptr<DataWrapper> getDataWrapper();

    void fillForm(std::vector<DataType> list);
    void setFormData(CompanionDataPtr data);

private:
    std::shared_ptr<QFormLayout> layout_;
    std::unordered_map<DataType, std::shared_ptr<FormLine>> map_;
};

class DataDialog : public Dialog, public FormLayout
{
    Q_OBJECT

public:
    DataDialog(const std::string &title, const std::vector<DataType> &lines);
    virtual ~DataDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;
    std::shared_ptr<DataWrapper> getData() override;

private:
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class CompanionDataDialog : public DataDialog
{
    Q_OBJECT

public:
    CompanionDataDialog(CompanionPtr companion, const std::string &title);
    ~CompanionDataDialog() { logArgsInfo(Q_FUNC_INFO); }

    CompanionDataPtr getCompanionData() override;
};

class UpdateCompanionDataDialog : public CompanionDataDialog
{
    Q_OBJECT

public:
    UpdateCompanionDataDialog(CompanionPtr companion, const std::string &title);
    ~UpdateCompanionDataDialog() { logArgsInfo(Q_FUNC_INFO); }
};

// TODO inherit from DataDialog
class GroupChatDataDialog : public Dialog
{
    Q_OBJECT

public:
    GroupChatDataDialog(const std::string &title);
    ~GroupChatDataDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;

private:
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QListWidget> list_;
};

class TextDialog : public Dialog
{
    Q_OBJECT

public:
    TextDialog(
        std::shared_ptr<Dialog> parent, const std::string &title, const std::string &text,
        const std::vector<ButtonInfo> &buttonsInfo);

    ~TextDialog();

    void set(ActionPtr action) override;
    void closeSelf();
    void closeSelfAndParent();
    void acceptAction();
    void setText(const std::string &text);
    void update(const std::string &text, const std::vector<ButtonInfo> &list);
    void updateButtonBox(const std::vector<ButtonInfo> &list);

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
    FileDialog(const std::string &title);
    ~FileDialog();

    void set(ActionPtr action) override;
    void showDialog() override;
    QStringList selectedFiles() const override;
    QString getSaveFileName(const QString &param, const QString &path) override;

private:
    std::shared_ptr<QFileDialog> dialog_;
};

#endif // WIDGETS_DIALOG_HPP
