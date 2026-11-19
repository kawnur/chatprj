#ifndef WIDGET_BUTTON_HPP
#define WIDGET_BUTTON_HPP

#include <QDialogButtonBox>
#include <QPushButton>
#include <QString>

#include "widget_info.hpp"
#include "widget_layout.hpp"

class Button : public QPushButton
{
    Q_OBJECT

public:
    Button(const QString &text);
    ~Button();
};

class StateButton : public Button
{
    Q_OBJECT

public:
    StateButton(const std::vector<QString> &texts);
    ~StateButton() = default;

    void updateState();

private:
    std::vector<QString> texts_;
};

class ButtonBox :public QDialogButtonBox
{
    Q_OBJECT

public:
    ButtonBox(const std::vector<ButtonInfo> &list);
    ~ButtonBox();

private:
    // std::vector<Button> buttons;

};

class ButtonPanelWidget : public HLayoutWidget
{
public:
    ButtonPanelWidget(WidgetType type, const std::vector<ButtonInfo> &list);
    ~ButtonPanelWidget() = default;

    void updateButtonState();

private:
    std::shared_ptr<StateButton> stateButton_;  // only one
    std::vector<std::shared_ptr<Button>> buttons_;
};

#endif // WIDGET_BUTTON_HPP
