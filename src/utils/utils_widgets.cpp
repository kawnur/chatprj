#include "utils_widgets.hpp"

#include "constants.hpp"
#include "widgets_dialog.hpp"

std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(Button value)
{
    auto vector = std::make_shared<std::vector<ButtonInfo>>();

    // auto text = getButtonText(value);
    // auto cancelText = getButtonText(Button::CANCEL);

    vector->emplace_back(value, QDialogButtonBox::AcceptRole, &TextDialog::acceptAction);
    vector->emplace_back(Button::CANCEL, QDialogButtonBox::RejectRole, &TextDialog::reject);

    return vector;
}

QString getButtonText(Button value)
{
    return getMapValue(BUTTON_ROLE_TO_TEXT_MAP, value, DEFAULT_BUTTON_TEXT);
}