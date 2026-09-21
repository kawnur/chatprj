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
#include <QString>

#include "constants.hpp"
#include "graphic_manager.hpp"
#include "utils.hpp"

using namespace std::string_literals;

class Action;
class Companion;
class FileAction;

template <typename T, typename F>
void showDialogAndLog(
    T &&message, F &&func, DialogType type, std::shared_ptr<QWidget> parent = nullptr)
{
    getGraphicManager()->createTextDialogAndShow(
        parent, type, message, createOkButtonInfoVector(func));

    logTypeArgs(getLogTypeByDialogType(type), message);
}

template <typename T, typename F>
void showInfoDialogAndLogInfo(T &&message, F &&func, std::shared_ptr<QWidget> parent = nullptr)
{
    showDialogAndLog(message, func, DialogType::INFO, parent);
}

template <typename T>
void showInfoDialogAndLogInfo(T &&message, std::shared_ptr<QWidget> parent = nullptr)
{
    showDialogAndLog(message, &QDialog::accept, DialogType::INFO, parent);
}

template <typename T>
void showWarningDialogAndLogWarning(T &&message, std::shared_ptr<QWidget> parent = nullptr)
{
    showDialogAndLog(message, &QDialog::accept, DialogType::WARNING, parent);
}

template <typename T>
void showErrorDialogAndLogError(T &&message, std::shared_ptr<QWidget> parent = nullptr)
{
    showDialogAndLog(message, &QDialog::accept, DialogType::ERROR, parent);
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog() = default;
    ~Dialog() = default;

    virtual void set() {}
    virtual void showDialog() {}

    void setAction(std::shared_ptr<Action> action) { action_ = action; }
    bool containsDialog() const { return containsDialog_; }

protected:
    bool containsDialog_ = false;
    std::shared_ptr<Action> action_;
};

class CompanionDataDialog : public Dialog
{
    Q_OBJECT

public:
    CompanionDataDialog(ChatActionType, std::shared_ptr<QWidget>, std::shared_ptr<Companion>);
    ~CompanionDataDialog() = default;

    std::string getNameString();
    std::string getIpAddressString();
    std::string getPortString();
    void set() override;

private:
    ChatActionType type_;
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
    GroupChatDataDialog(ChatActionType, std::shared_ptr<QWidget>);
    ~GroupChatDataDialog() = default;

    void set() override;

private:
    ChatActionType type_;
    std::unique_ptr<QVBoxLayout> layout_;
    std::unique_ptr<QLabel> label_;
    std::unique_ptr<QListWidget> list_;
};

class CreatePasswordDialog : public Dialog
{
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

class GetPasswordDialog : public Dialog
{
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
    ButtonInfo(
        const QString &text, QDialogButtonBox::ButtonRole role,
        std::function<void(TextDialog  &)> function);

    ~ButtonInfo() = default;

    QString getText();
    QDialogButtonBox::ButtonRole getRole();

    // void (TextDialog::*function_)();
    std::function<void(TextDialog  &)> getFunction();

private:
    QString text_;
    QDialogButtonBox::ButtonRole role_;
    std::function<void(TextDialog  &)> function_;
};

class TextDialog : public Dialog
{
    Q_OBJECT

public:
    TextDialog(
        std::shared_ptr<QWidget> parent, DialogType type, const std::string &text,
        std::shared_ptr<std::vector<ButtonInfo>> buttonsInfo);

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

class FileDialog : public Dialog
{
    Q_OBJECT

public:
    FileDialog(std::shared_ptr<FileAction> action, const std::string &title);
    ~FileDialog() = default;

    void set() override;
    void showDialog() override;

    std::shared_ptr<QFileDialog> getFileDialog();

private:
    std::shared_ptr<FileAction> action_;
    std::shared_ptr<QFileDialog> dialog_;
};

template<class T>
void setButtonBox(
    std::shared_ptr<T> dialog, std::shared_ptr<QDialogButtonBox> buttonBox,
    std::vector<ButtonInfo> *infoVector)
{
    const std::map<QDialogButtonBox::ButtonRole, std::function<void()>> signalMap {
        { QDialogButtonBox::ButtonRole::AcceptRole, &QDialogButtonBox::accepted },
        { QDialogButtonBox::ButtonRole::RejectRole, &QDialogButtonBox::rejected }
    };

    for (auto &info : *infoVector) {
        auto role = info.getRole();
        auto function = info.getFunction();
        auto button = buttonBox->addButton(info.getText(), role);

        auto connectLambda = [&]()
        {
            QObject::connect(
                buttonBox.get(), signalMap.at(role), dialog, function, Qt::QueuedConnection);
        };

        auto handlerLambda = [&](const std::exception &e)
        {
            if (dynamic_cast<const std::out_of_range *>(&e))
                showErrorDialogAndLogError("Unmanaged button role"s);
        };

        runAndHandleException(connectLambda, handlerLambda, role);
    }
}

std::shared_ptr<std::vector<ButtonInfo>> createOkButtonInfoVector(
    std::function<void(TextDialog &)> function);

#endif // WIDGETS_DIALOG_HPP
