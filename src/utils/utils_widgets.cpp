#include "utils_widgets.hpp"

#include <unordered_map>

#include "widgets.hpp"

const std::unordered_map<WidgetType, uint32_t> WIDGET_LIGHTNESS_MAP {
    { WidgetType::MAIN_WINDOW_MENU, 45 },
    { WidgetType::LEFT_PANEL, 70 },
    { WidgetType::SOCKET_INFO, 55 },
    { WidgetType::SOCKET_INFO_SELECTED, 40 },
    { WidgetType::COMPANION_NAME_LABEL, 60 },
    { WidgetType::SHOW_HIDE, 30 },
    { WidgetType::MESSAGE, 80 },
    { WidgetType::HISTORY, 75 },
    { WidgetType::BUTTON_PANEL, 65 },
    { WidgetType::TEXT_EDIT, 80 },
    { WidgetType::APP_LOG, 85 },
    { WidgetType::INDICATOR_ME, 30 }
};

const std::unordered_map<WidgetType, uint32_t> WIDGET_COLOR_MAP {
    { WidgetType::MESSAGE_SENT, 0x115e00 },
    { WidgetType::MESSAGE_RECEIVED, 0x00115e },
    { WidgetType::FILE_SENT, 0x093400 },
    { WidgetType::FILE_RECEIVED, 0x000934 },
    { WidgetType::INDICATOR_ON, 0x009600 },
    { WidgetType::INDICATOR_OFF, 0x690000 },
    { WidgetType::NEW_MESSAGE_EDIT, 0xdcdc07 }
};

// TODO add color calculation instead of color map
const std::unordered_map<uint32_t, uint32_t> WIDGET_LIGHTNESS_TO_COLOR_MAP {
    { 0, 0x000000 },
    { 5, 0x0d0d0c },
    { 10, 0x1b1a18 },
    { 15, 0x282725 },
    { 20, 0x353531 },
    { 25, 0x42423d },
    { 30, 0x504f49 },
    { 35, 0x5d5c56 },
    { 40, 0x6a6962 },
    { 45, 0x78766e },
    { 50, 0x85837a },
    { 55, 0x929186 },
    { 60, 0x9f9e93 },
    { 65, 0xadab9f },
    { 70, 0xbab8ab },
    { 75, 0xc6c4b9 },
    { 80, 0xd1d0c7 },
    { 85, 0xdddcd5 },
    { 90, 0xe8e7e3 },
    { 95, 0xf4f3f1 },
    { 100, 0xffffff }
};

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

MessageWidgetType getMessageWidgetType(MessageType value)
{
    return getMapValue(MESSAGE_WIDGET_TYPE_MAP, value, MessageWidgetType::UNKNOWN);
}

// color
uint32_t getWidgetColor(WidgetType type)
{
    // get color
    if (WIDGET_COLOR_MAP.contains(type))
        return WIDGET_COLOR_MAP.at(type);

    // or get color by lightness
    auto lightness = getMapValue(WIDGET_LIGHTNESS_MAP, type, DEFAULT_WIDGET_LIGHTNESS);

    return getMapValue(WIDGET_LIGHTNESS_TO_COLOR_MAP, lightness, DEFAULT_WIDGET_COLOR);
}

std::string getHTMLColor(WidgetType type)
{
    return std::format("#{:06x}", getWidgetColor(type));
}

QString getHTMLHeader(
    const std::string &color, const std::string &sender, const std::string &receiver,
    const std::string &time)
{
    auto headerTemplate = "<font color=\"{0}\"><b><br><i>From {1} to {2} at {3}:</i></b></font>"s;

    return getQString(getStringByFormat(headerTemplate, color, sender, receiver, time));
}

QString getHTMLBody(const std::string &color, const std::string &text, bool _break, bool bold)
{
    auto breakSub = (_break) ? "<br>"s : ""s;
    auto boldOpen = (bold) ? "<b>"s : ""s;
    auto boldClose = (bold) ? "</b>"s : ""s;

    auto textSub = getStringByFormat("{0}{1}{2}{3}", breakSub, boldOpen, text, boldClose);

    return getQString(getStringByFormat("<font color=\"{0}\">{1}</font>", color, textSub));
}
