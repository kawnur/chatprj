#include "utils_widgets.hpp"

#include "constants.hpp"
#include "widgets_dialog.hpp"

std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(const QString& buttonText)
{
    auto vector = std::make_shared<std::vector<ButtonInfo>>();

    vector->emplace_back(cancelButtonText, QDialogButtonBox::RejectRole, &TextDialog::reject);
    vector->emplace_back(buttonText, QDialogButtonBox::AcceptRole, &TextDialog::acceptAction);

    return vector;
}
