#include "widget_layout.hpp"

#include "logging.hpp"
#include "message.hpp"
#include "message_widget.hpp"
#include "utils_widgets.hpp"

Layout::Layout(WidgetType type) : type_(type)
{
    spacing_ = getLayoutSpacingValue(type_);

    // margins
    leftMargin_ = getLayoutContentsLeftMargin(type);
    topMargin_ = getLayoutContentsTopMargin(type);
    rightMargin_ = getLayoutContentsRightMargin(type);
    bottomMargin_ = getLayoutContentsBottomMargin(type);
}

VLayout::VLayout(WidgetType type, Qt::AlignmentFlag alignment)
    : Layout(type), layout_(new QVBoxLayout)
{
    layout_->setSpacing(spacing_);
    layout_->setContentsMargins(leftMargin_, topMargin_, rightMargin_, bottomMargin_);

    if (alignment == Qt::AlignCenter)
        layout_->setAlignment(alignment);
    else
        layout_->setAlignment(alignment | Qt::AlignTop);

    layout_->setSizeConstraint(QLayout::SetMaximumSize);
}

void VLayout::addWidget(QWidget *widget)
{
    layout_->addWidget(widget);
}

void VLayout::removeWidget(QWidget *widget)
{
    layout_->removeWidget(widget);
}

void VLayout::logLayout()
{
    logArgsInfo(LAYOUT_LOG_DELIMITER);

    for (std::size_t i = 0; i < layout_->count(); ++i)
        logArgsInfo(layout_->itemAt(i)->widget());

    logArgsInfo(LAYOUT_LOG_DELIMITER);
}

void VLayout::insertInSortedLayout(MessageWidgetPtr widget)
{
    auto time = widget->message()->time();
    auto count = layout_->count();

    if (count == 0) {
        layout_->addWidget(widget.get());

        return;
    }

    std::size_t index = count - 1;

    for (; index >= 0 && index != SIZE_MAX; --index) {
        auto item = layout_->itemAt(index)->widget();
        auto cast = dynamic_cast<MessageWidget *>(item);

        if (!cast)
            continue;

        auto itemTime = cast->message()->time();

        if (time > itemTime)
            break;
    }

    layout_->insertWidget(index + 1, widget.get());
}

HLayout::HLayout(WidgetType type, Qt::AlignmentFlag alignment)
    : Layout(type), layout_(new QHBoxLayout)
{
    layout_->setSpacing(spacing_);
    layout_->setContentsMargins(leftMargin_, topMargin_, rightMargin_, bottomMargin_);
    layout_->setAlignment(alignment | Qt::AlignTop);
    layout_->setSizeConstraint(QLayout::SetMaximumSize);
}

void HLayout::addWidget(QWidget *widget)
{
    layout_->addWidget(widget);
}

void HLayout::removeWidget(QWidget *widget)
{
    layout_->removeWidget(widget);
}

void HLayout::logLayout()
{
    logArgsInfo(LAYOUT_LOG_DELIMITER);

    for (std::size_t i = 0; i < layout_->count(); ++i)
        logArgsInfo(layout_->itemAt(i)->widget());

    logArgsInfo(LAYOUT_LOG_DELIMITER);
}

VLayoutWidget::VLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment)
    : PalettedWidget(type), VLayout(type, alignment)
{
    setLayout(layout_.get());
}

HLayoutWidget::HLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment)
    : PalettedWidget(type), HLayout(type, alignment)
{
    setLayout(layout_.get());
}
