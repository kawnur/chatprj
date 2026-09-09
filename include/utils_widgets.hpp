#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <memory>

#include <QObject>

#include "constants.hpp"
#include "utils.hpp"
#include "widgets.hpp"
#include "widgets_dialog.hpp"

void showErrorDialogAndLogError(QString&& message);

class ButtonInfo;

std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(const QString&);

// template<class T>
// void setButtonBox(
//     std::shared_ptr<T> dialog, std::shared_ptr<QDialogButtonBox> buttonBox,
//     std::vector<ButtonInfo>* infoVector)
// {
//     for (auto& info : *infoVector) {
//         auto role = info.getRole();
//         auto button = buttonBox->addButton(info.getText(), role);

//         // TODO create mapping and select signal by role
//         if (role == QDialogButtonBox::AcceptRole) {
//             QObject::connect(
//                 buttonBox.get(), &QDialogButtonBox::accepted,
//                 dialog, info.function_, Qt::QueuedConnection);
//         }
//         else if (role == QDialogButtonBox::RejectRole) {
//             QObject::connect(
//                 buttonBox.get(), &QDialogButtonBox::rejected,
//                 dialog, info.function_, Qt::QueuedConnection);
//         }
//         else {
//             showErrorDialogAndLogError("Unmanaged button role");
//         }
//     }
// }

#endif // UTILS_WIDGETS_HPP
