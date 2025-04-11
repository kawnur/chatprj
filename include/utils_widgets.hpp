#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include "constants.hpp"
#include "utils.hpp"
#include "widgets.hpp"

class ButtonInfo;
void showErrorDialogAndLogError(QString&&, QWidget* = nullptr);

std::vector<ButtonInfo>* getButtonInfoVectorPtr(const QString&);

template<class T>
void setButtonBox(
    T* dialogPtr, QDialogButtonBox* buttonBoxPtr,
    std::vector<ButtonInfo>* infoVector) {

    for(auto& info : *infoVector) {
        QPushButton* buttonPtr = buttonBoxPtr->addButton(
            info.buttonText_, info.buttonRole_);

        // TODO create mapping and select signal by role
        if(info.buttonRole_ == QDialogButtonBox::AcceptRole) {
            connect(
                buttonBoxPtr, &QDialogButtonBox::accepted,
                dialogPtr, info.functionPtr_, Qt::QueuedConnection);
        }
        else if(info.buttonRole_ == QDialogButtonBox::RejectRole) {
            connect(
                buttonBoxPtr, &QDialogButtonBox::rejected,
                dialogPtr, info.functionPtr_, Qt::QueuedConnection);
        }
        else {
            showErrorDialogAndLogError("Unmanaged button role");
        }
    }
}

#endif // UTILS_WIDGETS_HPP
