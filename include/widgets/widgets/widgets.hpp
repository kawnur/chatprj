#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <QLabel>
#include <QScrollArea>
#include <QSplitter>

#include "widget_layout.hpp"
#include "widget_palette.hpp"

class LabelWidget : public QLabel, public Palette
{
    Q_OBJECT

public:
    LabelWidget(WidgetType type, const QString &text);
    ~LabelWidget() = default;
};

class SplitterWidget : public QSplitter, public Palette
{
    Q_OBJECT

public:
    SplitterWidget(WidgetType type, QWidget *parent = nullptr);
    ~SplitterWidget() = default;
};

class IndicatorWidget : public PalettedWidget
{
    Q_OBJECT

public:
    IndicatorWidget(WidgetType type, IndicatorWidgetState state);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();
    void updateState();

private:
    IndicatorWidgetState state_;
    uint8_t size_;
};

class ShowHideWidget : public VLayoutWidget
{
    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget() = default;

private:
    void hideInfo();
    void showInfo();
    void mousePressEvent(QMouseEvent *event) override;

    bool show_;
    std::unique_ptr<QLabel> label_;
};

class ScrollAreaWidget : public QScrollArea
{
    Q_OBJECT

public:
    ScrollAreaWidget(QWidgetPtr widget);
    ~ScrollAreaWidget() = default;
};

#endif // WIDGETS_HPP
