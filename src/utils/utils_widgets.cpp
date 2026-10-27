#include "utils_widgets.hpp"

#include "constants.hpp"
#include "widgets.hpp"

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

QString getFileWidgetText(WidgetType value)
{
    return getMapValue(FILE_WIDGET_TEXT_MAP, value, DEFAULT_FILE_WIDGET_TEXT);
}
