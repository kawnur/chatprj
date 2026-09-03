#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <memory>

#include "constants.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

void showErrorDialogAndLogError(QString&&, std::shared_ptr<QWidget> = nullptr);

std::vector<ButtonInfo>* getButtonInfoVector(const QString&);

template<class T>
void setButtonBox(
    std::shared_ptr<T> dialog, std::shared_ptr<QDialogButtonBox> buttonBox,
    std::vector<ButtonInfo>* infoVector)
{
    for(auto& info : *infoVector) {
        std::shared_ptr<QPushButton> button = buttonBox->addButton(
            info.buttonText_, info.buttonRole_);

        // TODO create mapping and select signal by role
        if(info.buttonRole_ == QDialogButtonBox::AcceptRole) {
            connect(
                buttonBox, &QDialogButtonBox::accepted,
                dialog, info.function_, Qt::QueuedConnection);
        }
        else if(info.buttonRole_ == QDialogButtonBox::RejectRole) {
            connect(
                buttonBox, &QDialogButtonBox::rejected,
                dialog, info.function_, Qt::QueuedConnection);
        }
        else {
            showErrorDialogAndLogError("Unmanaged button role");
        }
    }
}

#endif // UTILS_WIDGETS_HPP
