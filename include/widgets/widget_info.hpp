#ifndef WIDGET_INFO_HPP
#define WIDGET_INFO_HPP

#include <functional>
#include <vector>

#include <QDialogButtonBox>
#include <QString>

class WidgetInfo
{
public:
    WidgetInfo();
    WidgetInfo(const std::vector<QString> &texts, std::function<void()> function);
    ~WidgetInfo();

    std::vector<QString> texts() const;
    std::function<void()> function() const;

protected:
    std::vector<QString> texts_;
    std::function<void()> function_;
};

class ButtonInfo : public WidgetInfo
{
public:
    ButtonInfo();

    ButtonInfo(
        const std::vector<QString> &texts, std::function<void()> function,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::ButtonRole::NoRole);

    ~ButtonInfo();

    QDialogButtonBox::ButtonRole role() const;

private:
    QDialogButtonBox::ButtonRole role_;
};

class MenuItemInfo : public WidgetInfo
{
public:
    MenuItemInfo();

    MenuItemInfo(
        const std::vector<QString> &texts, std::function<void()> function,
        bool isSwitchable = false, bool isDisabled = false);

    ~MenuItemInfo();

    bool isSwitchable() const;
    bool isDisabled() const;

private:
    bool isSwitchable_;
    bool isDisabled_;
};

#endif // WIDGET_INFO_HPP
