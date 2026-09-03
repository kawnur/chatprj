#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

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

    void setAction(std::shared_ptr<Action> actionPtr) { this->actionPtr_ = actionPtr; }
    virtual void set() {}

    bool getContainsDialogPtr() const { return this->containsDialogPtr_; }
    virtual void showDialog() {}

protected:
    bool containsDialogPtr_ = false;
    std::shared_ptr<Action> actionPtr_;
};

class CompanionDataDialog : public Dialog {

    Q_OBJECT

public:
    CompanionDataDialog(ChatActionType, std::shared_ptr<QWidget>, std::shared_ptr<Companion>);
    ~CompanionDataDialog();

    void set() override;
    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();

private:
    ChatActionType actionType_;
    std::shared_ptr<QFormLayout> layoutPtr_;
    std::shared_ptr<QLabel> nameLabelPtr_;
    std::shared_ptr<QLineEdit> nameEditPtr_;
    std::shared_ptr<QLabel> ipAddressLabelPtr_;
    std::shared_ptr<QLineEdit> ipAddressEditPtr_;
    std::shared_ptr<QLabel> portLabelPtr_;
    std::shared_ptr<QLineEdit> portEditPtr_;
    std::shared_ptr<QDialogButtonBox> buttonBoxPtr_;
};

class GroupChatDataDialog : public Dialog {

    Q_OBJECT

public:
    GroupChatDataDialog(ChatActionType, std::shared_ptr<QWidget>);
    ~GroupChatDataDialog();

    void set() override;

private:
    ChatActionType actionType_;
    std::shared_ptr<QVBoxLayout> layoutPtr_;
    std::shared_ptr<QLabel> labelPtr_;
    std::shared_ptr<QListWidget> listPtr_;

};

class CreatePasswordDialog : public Dialog {

    Q_OBJECT

public:
    CreatePasswordDialog();
    ~CreatePasswordDialog();

    void set() override;
    std::string getFirstEditText();
    std::string getSecondEditText();

private:
    std::shared_ptr<QFormLayout> layoutPtr_;
    std::shared_ptr<QLabel> firstLabelPtr_;
    std::shared_ptr<QLineEdit> firstEditPtr_;
    std::shared_ptr<QLabel> secondLabelPtr_;
    std::shared_ptr<QLineEdit> secondEditPtr_;
    std::shared_ptr<QDialogButtonBox> buttonBoxPtr_;
};

class GetPasswordDialog : public Dialog {

    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog();

    void set() override;
    std::string getEditText();

private:
    std::shared_ptr<QFormLayout> layoutPtr_;
    std::shared_ptr<QLabel> labelPtr_;
    std::shared_ptr<QLineEdit> editPtr_;
    std::shared_ptr<QDialogButtonBox> buttonBoxPtr_;
};

class TextDialog;

class ButtonInfo {
public:
    ButtonInfo(
        const QString&, QDialogButtonBox::ButtonRole, void (TextDialog::*)());

    ~ButtonInfo() = default;

    QString buttonText_;
    QDialogButtonBox::ButtonRole buttonRole_;
    void (TextDialog::*functionPtr_)();
};

class TextDialog : public Dialog {

    Q_OBJECT

public:
    TextDialog(std::shared_ptr<QWidget>, DialogType, const std::string&, std::vector<ButtonInfo>*);
    ~TextDialog();

    void set() override;
    void closeSelf();
    void closeSelfAndParentDialog();
    void acceptAction();

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    std::shared_ptr<QPlainTextEdit> textEditPtr_;
    std::shared_ptr<QVBoxLayout> layoutPtr_;
    std::shared_ptr<QDialogButtonBox> buttonBoxPtr_;
    std::vector<ButtonInfo>* buttonsInfoPtr_;
};

class FileDialog : public Dialog {

    Q_OBJECT

public:
    FileDialog(std::shared_ptr<FileAction>, const QString&);
    ~FileDialog();

    void set();
    void showDialog() override;

    std::shared_ptr<QFileDialog> getFileDialogPtr();

private:
    std::shared_ptr<FileAction> actionPtr_;
    std::shared_ptr<QFileDialog> fileDialogPtr_;
};

#endif // WIDGETS_DIALOG_HPP
