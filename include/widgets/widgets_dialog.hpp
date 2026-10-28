#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>

#include "alias.hpp"
#include "constants.hpp"
#include "logging.hpp"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog();
    virtual ~Dialog() { logArgsInfo(Q_FUNC_INFO); }

    virtual void set(ActionPtr action) {}
    virtual void showDialog() {}
    virtual CompanionDataPtr getCompanionData() { return nullptr; }
    virtual std::string firstEditText() { return ""s; }
    virtual std::string secondEditText() { return ""s; }
    virtual std::string editText() { return ""s; }
    virtual QStringList selectedFiles() const { return QStringList(); }
    virtual QString getSaveFileName(const QString &param, const QString &path) { return ""; }

    // bool containsDialog() const { return containsDialog_; }
    void setDialogType(DialogType type);

// protected slots:
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

class CompanionDataDialog : public Dialog
{
    Q_OBJECT

public:
    CompanionDataDialog(ActionType type, CompanionPtr companion);
    ~CompanionDataDialog() { logArgsInfo(Q_FUNC_INFO); }

    std::string name();
    std::string ipAddress();
    std::string port();

    void set(ActionPtr action) override;

    CompanionDataPtr getCompanionData() override;

private:
    ActionType type_;
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> nameLabel_;
    std::unique_ptr<QLineEdit> nameEdit_;
    std::unique_ptr<QLabel> ipAddressLabel_;
    std::unique_ptr<QLineEdit> ipAddressEdit_;
    std::unique_ptr<QLabel> portLabel_;
    std::unique_ptr<QLineEdit> portEdit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
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

class CreatePasswordDialog : public Dialog
{
    Q_OBJECT

public:
    CreatePasswordDialog();
    ~CreatePasswordDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;
    std::string firstEditText() override;
    std::string secondEditText() override;

private:
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> firstLabel_;
    std::unique_ptr<QLineEdit> firstEdit_;
    std::unique_ptr<QLabel> secondLabel_;
    std::unique_ptr<QLineEdit> secondEdit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class GetPasswordDialog : public Dialog
{
    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog() { logArgsInfo(Q_FUNC_INFO); }

    void set(ActionPtr action) override;
    std::string editText() override;

private:
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QLineEdit> edit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class TextDialog;

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
