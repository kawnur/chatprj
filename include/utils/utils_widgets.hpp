#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <memory>
#include <vector>

#include <QString>
#include <QWidget>

#include "graphic_manager.hpp"
#include "widgets_dialog.hpp"

class ButtonInfo;

// std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(ButtonType value);
ButtonInfo getOKButtonInfo(const std::function<void()> &function);
QString getButtonText(ButtonType value);
QDialogButtonBox::ButtonRole getButtonRole(ButtonType type);

#endif // UTILS_WIDGETS_HPP
