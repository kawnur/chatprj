#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

ButtonInfo getOKButtonInfo(const std::function<void()> &function);
ButtonInfo getCancelButtonInfo(const std::function<void()> &function);
QString getButtonText(ButtonType value);
QDialogButtonBox::ButtonRole getButtonRole(ButtonType type);

#endif // UTILS_WIDGETS_HPP
