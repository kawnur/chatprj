#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <functional>
#include <string>

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

ButtonInfo getOKButtonInfo(const std::function<void()> &function);
ButtonInfo getCancelButtonInfo(const std::function<void()> &function);
QString getButtonText(ButtonType value);
QDialogButtonBox::ButtonRole getButtonRole(ButtonType type);
QString getFileWidgetText(WidgetType value);
MessageWidgetType getMessageWidgetType(MessageType value);

// color
uint32_t getWidgetColor(WidgetType value);
std::string getHTMLColor(WidgetType value);

QString getHTMLHeader(
    const std::string &color, const std::string &sender, const std::string &receiver,
    const std::string &time);

QString getHTMLBody(const std::string &color, const std::string &text, bool _break, bool bold);
uint32_t getLayoutValue(WidgetType type);
uint32_t getIndicatorSize(WidgetType type);

#endif // UTILS_WIDGETS_HPP
