#ifndef WIDGET_MENU_HPP
#define WIDGET_MENU_HPP

#include <QAction>
#include <QMenu>

#include "widget_info.hpp"

class MenuItem : public QAction
{
    Q_OBJECT

public:
    MenuItem(const QString &text, bool isSwitchable, bool isDisabled);
    ~MenuItem();

    bool isSwitchable() const;

private:
    bool isSwitchable_;
};

class Menu : public QMenu
{
public:
    Menu(const std::vector<MenuItemInfo> &info);
    ~Menu();

    void updateSwitchableItems();

private:
    std::vector<std::shared_ptr<MenuItem>> items_;
};

#endif // WIDGET_MENU_HPP
