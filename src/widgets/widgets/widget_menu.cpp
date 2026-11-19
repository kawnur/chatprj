#include "widget_menu.hpp"

#include "logging.hpp"

MenuItem::MenuItem(const QString &text, bool isSwitchable, bool isDisabled)
    : QAction(text), isSwitchable_(isSwitchable)
{
    setDisabled(isDisabled);
}

MenuItem::~MenuItem() { logArgsInfo(Q_FUNC_INFO); }

bool MenuItem::isSwitchable() const
{
    return isSwitchable_;
}

Menu::Menu(const std::vector<MenuItemInfo> &info) : QMenu(), items_()
{
    for (const auto &item : info) {
        auto text = getIndexedContainerValue(item.texts(), 0, "");
        auto menuItem = std::make_shared<MenuItem>(text, item.isSwitchable(), item.isDisabled());
        addAction(menuItem.get());
        items_.push_back(menuItem);

        connect(menuItem.get(), &QAction::triggered, this, item.function(), Qt::QueuedConnection);
    }
}

Menu::~Menu() { logArgsInfo(Q_FUNC_INFO); }

void Menu::updateSwitchableItems()
{
    for (auto &item : items_) {
        if (item->isSwitchable())
            item->resetEnabled();
    }
}
