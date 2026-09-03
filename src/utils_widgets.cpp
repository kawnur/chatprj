#include "utils_widgets.hpp"

std::vector<ButtonInfo>* getButtonInfoVector(const QString& buttonText) {
    auto vector = new std::vector<ButtonInfo> {
        ButtonInfo(cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
        ButtonInfo(buttonText, QDialogButtonBox::AcceptRole, &TextDialog::acceptAction)
    };

    return vector;
}
