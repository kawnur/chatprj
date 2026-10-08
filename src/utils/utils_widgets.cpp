#include "utils_widgets.hpp"

#include "constants.hpp"
#include "widgets.hpp"
// #include "widgets_dialog.hpp"

// std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(ButtonType value)
// {
//     auto vector = std::make_shared<std::vector<ButtonInfo>>();

//     // auto text = getButtonText(value);
//     // auto cancelText = getButtonText(Button::CANCEL);

//     vector->emplace_back(value, QDialogButtonBox::AcceptRole, &TextDialog::acceptAction);
//     vector->emplace_back(ButtonType::CANCEL, QDialogButtonBox::RejectRole, &TextDialog::reject);

//     return vector;
// }

ButtonInfo getOKButtonInfo(const std::function<void()> &function)
{
    return ButtonInfo(ButtonType::OK, function);
}

ButtonInfo getCancelButtonInfo(const std::function<void()> &function)
{
    return ButtonInfo(ButtonType::CANCEL, function);
}

QString getButtonText(ButtonType value)
{
    return getMapValue(BUTTON_TYPE_TO_TEXT_MAP, value, DEFAULT_BUTTON_TEXT);
}

QDialogButtonBox::ButtonRole getButtonRole(ButtonType type)
{
    return getMapValue(BUTTON_TYPE_TO_ROLE_MAP, type, DEFAULT_BUTTON_ROLE);
}
