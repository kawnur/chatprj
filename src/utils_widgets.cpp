#include "utils_widgets.hpp"

std::vector<ButtonInfo>* getButtonInfoVectorPtr(const QString& buttonText)
{
    auto vectorPtr = new std::vector<ButtonInfo>
    {
        ButtonInfo(cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject),
        ButtonInfo(buttonText, QDialogButtonBox::AcceptRole, &TextDialog::acceptAction)
    };

    return vectorPtr;
}
