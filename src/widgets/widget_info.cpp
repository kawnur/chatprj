#include "widget_info.hpp"

#include "logging.hpp"

WidgetInfo::WidgetInfo(): function_() {}

WidgetInfo::WidgetInfo(
    const std::vector<QString> &texts, std::function<void ()> function)
    : texts_(std::move(texts)), function_(function) {}

WidgetInfo::~WidgetInfo() { logArgsInfo(Q_FUNC_INFO); }

std::vector<QString> WidgetInfo::texts() const
{
    return texts_;
}

std::function<void()> WidgetInfo::function() const
{
    return function_;
}

ButtonInfo::ButtonInfo() : WidgetInfo() {}

ButtonInfo::ButtonInfo(
    const std::vector<QString> &texts, std::function<void ()> function,
    QDialogButtonBox::ButtonRole role)
    : WidgetInfo(texts, function), role_(role) {}

ButtonInfo::~ButtonInfo() { logArgsInfo(Q_FUNC_INFO); }

QDialogButtonBox::ButtonRole ButtonInfo::role() const
{
    return role_;
}

MenuItemInfo::MenuItemInfo() : WidgetInfo() {}

MenuItemInfo::MenuItemInfo(
    const std::vector<QString> &texts, std::function<void ()> function, bool isSwitchable,
    bool isDisabled)
    : WidgetInfo(texts, function), isSwitchable_(isSwitchable), isDisabled_(isDisabled) {}

MenuItemInfo::~MenuItemInfo() { logArgsInfo(Q_FUNC_INFO); }

bool MenuItemInfo::isSwitchable() const
{
    return isSwitchable_;
}

bool MenuItemInfo::isDisabled() const
{
    return isDisabled_;
}
