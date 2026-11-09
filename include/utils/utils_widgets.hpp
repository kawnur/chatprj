#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <functional>
#include <string>

#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

using LayoutContentsMarginMap = std::unordered_map<WidgetType, uint32_t>;

ButtonInfo getOKButtonInfo(const std::function<void()> &function);
ButtonInfo getCancelButtonInfo(const std::function<void()> &function);
QString getButtonText(ButtonType value);
QString getButtonText(ButtonState state);
QDialogButtonBox::ButtonRole getButtonRole(ButtonType type);
QString getFileWidgetText(WidgetType value);
// MessageWidgetType getMessageWidgetType(MessageType value);
QString getDataTypeStringRepr(DataType type);

// color
uint32_t getWidgetColor(WidgetType value);
uint32_t getIndicatorWidgetColor(IndicatorWidgetState state);

// html
std::string getHTMLColor(WidgetType value);

QString getHTMLHeader(
    const std::string &color, const std::string &sender, const std::string &receiver,
    const std::string &time);

QString getHTMLBody(const std::string &color, const std::string &text, bool _break, bool bold);
std::string getHTMLBold(const std::string &value);
std::string getHTMLItalic(const std::string &value);
std::string getHTMLBreak(const std::string &value);

uint32_t getLayoutSpacingValue(WidgetType type);

// layout contents margins
uint32_t getLayoutContentsLeftMargin(WidgetType type);
uint32_t getLayoutContentsTopMargin(WidgetType type);
uint32_t getLayoutContentsRightMargin(WidgetType type);
uint32_t getLayoutContentsBottomMargin(WidgetType type);

uint32_t getIndicatorSize(WidgetType type);

#endif // UTILS_WIDGETS_HPP
