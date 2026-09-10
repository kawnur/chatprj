#ifndef WIDGETS_DIALOG_HPP
#define WIDGETS_DIALOG_HPP

#include <functional>
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
#include <QString>

#include "constants.hpp"

void showErrorDialogAndLogError(QString &&message);

class Action;
class Companion;
class FileAction;

class Dialog : public QDialog {

    Q_OBJECT

public:
    Dialog() = default;
    ~Dialog() = default;

    void setAction(std::shared_ptr<Action> action) { action_ = action; }
    virtual void set() {}

    bool containsDialog() const { return containsDialog_; }
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
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> nameLabel_;
    std::unique_ptr<QLineEdit> nameEdit_;
    std::unique_ptr<QLabel> ipAddressLabel_;
    std::unique_ptr<QLineEdit> ipAddressEdit_;
    std::unique_ptr<QLabel> portLabel_;
    std::unique_ptr<QLineEdit> portEdit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class GroupChatDataDialog : public Dialog {

    Q_OBJECT

public:
    GroupChatDataDialog(ChatActionType, std::shared_ptr<QWidget>);
    ~GroupChatDataDialog() = default;

    void set() override;

private:
    ChatActionType actionType_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QListWidget> list_;
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
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> firstLabel_;
    std::unique_ptr<QLineEdit> firstEdit_;
    std::unique_ptr<QLabel> secondLabel_;
    std::unique_ptr<QLineEdit> secondEdit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class GetPasswordDialog : public Dialog {

    Q_OBJECT

public:
    GetPasswordDialog();
    ~GetPasswordDialog() = default;

    void set() override;
    std::string getEditText();

private:
    std::unique_ptr<QFormLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QLineEdit> edit_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
};

class TextDialog;

class ButtonInfo
{
public:
    // ButtonInfo(const QString&, QDialogButtonBox::ButtonRole, void (TextDialog::*)());
    ButtonInfo(const QString &text, QDialogButtonBox::ButtonRole role, std::function<void(TextDialog &)> function);
    ~ButtonInfo() = default;

    QString getText();
    QDialogButtonBox::ButtonRole getRole();

    // void (TextDialog::*function_)();
    std::function<void(TextDialog &)> getFunction();

private:
    QString text_;
    QDialogButtonBox::ButtonRole role_;
    std::function<void(TextDialog &)> function_;
};

class TextDialog : public Dialog {

    Q_OBJECT

public:
    TextDialog(
        std::shared_ptr<QWidget>, DialogType, const std::string&,
        std::shared_ptr<std::vector<ButtonInfo>>);

    ~TextDialog() = default;

    void set() override;
    void closeSelf();
    void closeSelfAndParentDialog();
    void acceptAction();

public slots:
    void unsetMainWindowBlurAndCloseDialogs();
    void reject() override;

private:
    std::unique_ptr<QPlainTextEdit> textEdit_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QDialogButtonBox> buttonBox_;
    std::shared_ptr<std::vector<ButtonInfo>> buttonsInfo_;
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

template<class T>
void setButtonBox(
    std::shared_ptr<T> dialog, std::shared_ptr<QDialogButtonBox> buttonBox,
    std::vector<ButtonInfo> *infoVector)
{
    for (auto &info : *infoVector) {
        auto role = info.getRole();
        auto function = info.getFunction();
        auto button = buttonBox->addButton(info.getText(), role);

        // TODO create mapping and select signal by role
        // if (role == QDialogButtonBox::AcceptRole) {
        //     QObject::connect(
        //         buttonBox.get(), &QDialogButtonBox::accepted,
        //         dialog, info.function_, Qt::QueuedConnection);
        // }
        // else if (role == QDialogButtonBox::RejectRole) {
        //     QObject::connect(
        //         buttonBox.get(), &QDialogButtonBox::rejected,
        //         dialog, info.function_, Qt::QueuedConnection);
        // }
        // else {
        //     showErrorDialogAndLogError("Unmanaged button role");
        // }

        std::map<QDialogButtonBox::ButtonRole, std::function<void()>> signalMap {
            { QDialogButtonBox::AcceptRole, &QDialogButtonBox::accepted },
            { QDialogButtonBox::RejectRole, &QDialogButtonBox::rejected }
        };

        auto connectLambda = [&]()
        {
            QObject::connect(
                buttonBox.get(), signalMap.at(role), dialog, function, Qt::QueuedConnection);
        };

        auto handlerLambda = [&](const std::exception &e)
        {
            if (dynamic_cast<std::out_of_range *>(&e))
                showErrorDialogAndLogError("Unmanaged button role");
        };

        runAndHandleException(connectLambda, handlerLambda, role);
    }
}

#endif // WIDGETS_DIALOG_HPP
