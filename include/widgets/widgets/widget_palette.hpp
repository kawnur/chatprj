#ifndef WIDGET_PALETTE_HPP
#define WIDGET_PALETTE_HPP

#include "constants.hpp"

class Palette
{
public:
    Palette(WidgetType type, QPalette::ColorRole role);
    ~Palette() = default;

    WidgetType type() const;

    void setType(WidgetType type);
    void setColor(uint32_t color);

protected:
    WidgetType type_;
    QPalette::ColorRole role_;
    std::unique_ptr<QPalette> palette_;
};

class PalettedWidget : public QWidget, public Palette
{
    Q_OBJECT

public:
    PalettedWidget(WidgetType type);
    virtual ~PalettedWidget() = default;

    void updateColor(uint32_t color);
    void updateType(WidgetType type);
};

#endif // WIDGET_PALETTE_HPP
