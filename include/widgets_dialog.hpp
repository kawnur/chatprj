#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
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

    void setAction(Action* actionPtr) { this->actionPtr_ = actionPtr; }
    virtual void set() {}

    bool getContainsDialogPtr() const { return this->containsDialogPtr_; }
    virtual void showDialog() {}

protected:
    bool containsDialogPtr_ = false;
    Action* actionPtr_;
};

class CompanionDataDialog : public Dialog {

    Q_OBJECT

public:
    CompanionDataDialog(ChatActionType, QWidget*, Companion*);
    ~CompanionDataDialog();

    void set() override;
    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();

private:
    ChatActionType actionType_;
    QFormLayout* layoutPtr_;
    QLabel* nameLabelPtr_;
    QLineEdit* nameEditPtr_;
    QLabel* ipAddressLabelPtr_;
    QLineEdit* ipAddressEditPtr_;
    QLabel* portLabelPtr_;
    QLineEdit* portEditPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class GroupChatDataDialog : public Dialog {

    Q_OBJECT

public:
    GroupChatDataDialog(ChatActionType, QWidget*);
    ~GroupChatDataDialog();

    void set() override;

private:
    ChatActionType actionType_;
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
    QFormLayout* layoutPtr_;
    QLabel* firstLabelPtr_;
    QLineEdit* firstEditPtr_;
    QLabel* secondLabelPtr_;
    QLineEdit* secondEditPtr_;
    QDialogButtonBox* buttonBoxPtr_;
};

class GetPasswordDialog : public Dialog {

    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog();

    void set() override;
    std::string getEditText();

private:
    QFormLayout* layoutPtr_;
    QLabel* labelPtr_;
    QLineEdit* editPtr_;
    QDialogButtonBox* buttonBoxPtr_;
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
    TextDialog(QWidget*, DialogType, const std::string&, std::vector<ButtonInfo>*);
    ~TextDialog();

    void set() override;
    void closeSelf();
    void closeSelfAndParentDialog();
    void acceptAction();

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    QPlainTextEdit* textEditPtr_;
    QVBoxLayout* layoutPtr_;
    QDialogButtonBox* buttonBoxPtr_;
    std::vector<ButtonInfo>* buttonsInfoPtr_;
};

class FileDialog : public Dialog {

    Q_OBJECT

public:
    FileDialog(FileAction*, const QString&);
    ~FileDialog();

    void set();
    void showDialog() override;

    QFileDialog* getFileDialogPtr();

private:
    FileAction* actionPtr_;
    QFileDialog* fileDialogPtr_;
};

#endif // WIDGETS_DIALOG_HPP
