#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

#include <memory>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>

#include "constants.hpp"
#include "logging.hpp"

class Action;
class Companion;
class FileAction;

class Dialog : public QDialog {

    Q_OBJECT

public:
    Dialog() = default;
    ~Dialog() = default;

    void setAction(std::shared_ptr<Action> action) { this->action_ = action; }
    virtual void set() {}

    bool getContainsDialog() const { return this->containsDialog_; }
    virtual void showDialog() {}

protected:
    bool containsDialog_ = false;
    std::shared_ptr<Action> action_;
};

class CompanionDataDialog : public Dialog {

    Q_OBJECT

public:
    CompanionDataDialog(ChatActionType, std::shared_ptr<QWidget>, std::shared_ptr<Companion>);
    ~CompanionDataDialog() = default;

    void set() override;
    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();

private:
    ChatActionType actionType_;
    std::shared_ptr<QFormLayout> layout_;
    std::shared_ptr<QLabel> nameLabel_;
    std::shared_ptr<QLineEdit> nameEdit_;
    std::shared_ptr<QLabel> ipAddressLabel_;
    std::shared_ptr<QLineEdit> ipAddressEdit_;
    std::shared_ptr<QLabel> portLabel_;
    std::shared_ptr<QLineEdit> portEdit_;
    std::shared_ptr<QDialogButtonBox> buttonBox_;
};

class GroupChatDataDialog : public Dialog {

    Q_OBJECT

public:
    GroupChatDataDialog(ChatActionType, std::shared_ptr<QWidget>);
    ~GroupChatDataDialog() = default;

    void set() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QListWidget> list_;

};

class CreatePasswordDialog : public Dialog {

    Q_OBJECT

public:
    CreatePasswordDialog();
    ~CreatePasswordDialog() = default;

    void set() override;
    std::string getFirstEditText();
    std::string getSecondEditText();

private:
    std::shared_ptr<QFormLayout> layout_;
    std::shared_ptr<QLabel> firstLabel_;
    std::shared_ptr<QLineEdit> firstEdit_;
    std::shared_ptr<QLabel> secondLabel_;
    std::shared_ptr<QLineEdit> secondEdit_;
    std::shared_ptr<QDialogButtonBox> buttonBox_;
};

class GetPasswordDialog : public Dialog {

    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog() = default;

    void set() override;
    std::string getEditText();

private:
    std::shared_ptr<QFormLayout> layout_;
    std::shared_ptr<QLabel> label_;
    std::shared_ptr<QLineEdit> edit_;
    std::shared_ptr<QDialogButtonBox> buttonBox_;
};

class TextDialog;

class ButtonInfo {
public:
    ButtonInfo(
        const QString&, QDialogButtonBox::ButtonRole, void (TextDialog::*)());

    ~ButtonInfo() = default;

    QString buttonText_;
    QDialogButtonBox::ButtonRole buttonRole_;
    void (TextDialog::*function_)();
};

class TextDialog : public Dialog {

    Q_OBJECT

public:
    TextDialog(std::shared_ptr<QWidget>, DialogType, const std::string&, std::vector<ButtonInfo>*);
    ~TextDialog() = default;

    void set() override;
    void closeSelf();
    void closeSelfAndParentDialog();
    void acceptAction();

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    std::shared_ptr<QPlainTextEdit> textEdit_;
    std::shared_ptr<QVBoxLayout> layout_;
    std::shared_ptr<QDialogButtonBox> buttonBox_;
    std::vector<ButtonInfo>* buttonsInfo_;
};

class FileDialog : public Dialog {

    Q_OBJECT

public:
    FileDialog(std::shared_ptr<FileAction>, const QString&);
    ~FileDialog() = default;

    void set();
    void showDialog() override;

    std::shared_ptr<QFileDialog> getFileDialog();

private:
    std::shared_ptr<FileAction> action_;
    std::shared_ptr<QFileDialog> fileDialog_;
};

#endif // WIDGETS_DIALOG_HPP
