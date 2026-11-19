#include "widget_palette.hpp"

#include "utils_widgets.hpp"

Palette::Palette(WidgetType type, QPalette::ColorRole role) : type_(type), role_(role)
{
    palette_ = std::make_unique<QPalette>();
    setColor(getWidgetColor(type_));
}

WidgetType Palette::type() const
{
    return type_;
}

void Palette::setType(WidgetType type)
{
    type_ = type;
    setColor(getWidgetColor(type_));
}

void Palette::setColor(uint32_t color)
{
    palette_->setColor(role_, QColor(color));
}

PalettedWidget::PalettedWidget(WidgetType type) : QWidget(), Palette(type, QPalette::Window)
{
    setAutoFillBackground(true);
    setPalette(*palette_);
}

void PalettedWidget::updateType(WidgetType type)
{
    setType(type);
    setPalette(*palette_);
}

void PalettedWidget::updateColor(uint32_t color)
{
    setColor(color);
    setPalette(*palette_);
}
