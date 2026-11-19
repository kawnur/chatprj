#include "widgets.hpp"

#include "graphic_manager.hpp"
#include "utils_widgets.hpp"

LabelWidget::LabelWidget(WidgetType type, const QString &text)
    : QLabel(text), Palette(type, QPalette::Window)
{
    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

SplitterWidget::SplitterWidget(WidgetType type, QWidget *parent) : Palette(type, QPalette::Window)
{
    if (parent)
        setParent(parent);

    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
}

IndicatorWidget::IndicatorWidget(WidgetType type, IndicatorWidgetState state)
    : PalettedWidget(type)
{
    size_ = getIndicatorSize(type_);
    setFixedWidth(size_);
    setFixedHeight(size_);
    state_ = state;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::setOn()
{
    state_ = IndicatorWidgetState::ON;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::setOff()
{
    state_ = IndicatorWidgetState::OFF;
    updateColor(getIndicatorWidgetColor(state_));
}

void IndicatorWidget::updateState()
{
    if (state_ == IndicatorWidgetState::OFF)
        state_ = IndicatorWidgetState::ON;
    else if (state_ == IndicatorWidgetState::ON)
        state_ = IndicatorWidgetState::OFF;

    updateColor(getIndicatorWidgetColor(state_));
}

ShowHideWidget::ShowHideWidget() : VLayoutWidget(WidgetType::SHOW_HIDE, Qt::AlignCenter)
{
    show_ = true;

    label_ = std::make_unique<QLabel>();
    label_->setText("Show / Hide");

    addWidget(label_.get());
}

void ShowHideWidget::hideInfo()
{
    getGraphicManager()->hideInfo();
}

void ShowHideWidget::showInfo()
{
    getGraphicManager()->showInfo();
}

void ShowHideWidget::mousePressEvent(QMouseEvent *event)
{
    (show_) ? hideInfo() : showInfo();
    show_ = !(show_);
}

ScrollAreaWidget::ScrollAreaWidget(QWidgetPtr widget)
{
    setWidgetResizable(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setWidget(widget.get());
}
