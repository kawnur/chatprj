#ifndef WIDGET_LAYOUT_HPP
#define WIDGET_LAYOUT_HPP

#include <memory>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "alias.hpp"
#include "constants.hpp"
#include "widget_palette.hpp"

class Layout
{
public:
    Layout(WidgetType type);
    virtual ~Layout() = default;

    virtual void addWidget(QWidget *widget) {}
    virtual void removeWidget(QWidget *widget) {}

protected:
    WidgetType type_;
    uint32_t spacing_;
    uint32_t leftMargin_;
    uint32_t topMargin_;
    uint32_t rightMargin_;
    uint32_t bottomMargin_;
};

class VLayout : public Layout
{
public:
    VLayout(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~VLayout() = default;

    void addWidget(QWidget *widget) override;
    void removeWidget(QWidget *widget) override;
    void logLayout();
    void insertInSortedLayout(MessageWidgetPtr widget);

protected:
    std::unique_ptr<QVBoxLayout> layout_;
};

class HLayout : public Layout
{
public:
    HLayout(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~HLayout() = default;

    void addWidget(QWidget *widget) override;
    void removeWidget(QWidget *widget) override;
    void logLayout();

protected:
    std::unique_ptr<QHBoxLayout> layout_;
};

class VLayoutWidget : public PalettedWidget, public VLayout
{
    Q_OBJECT

public:
    VLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~VLayoutWidget() = default;
};

class HLayoutWidget : public PalettedWidget, public HLayout
{
    Q_OBJECT

public:
    HLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~HLayoutWidget() = default;
};

#endif // WIDGET_LAYOUT_HPP
