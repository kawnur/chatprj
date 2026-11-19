#include "widget_button.hpp"

#include "logging.hpp"

Button::Button(const QString &text) : QPushButton(text) {}

Button::~Button() { logArgsInfo(Q_FUNC_INFO); }

StateButton::StateButton(const std::vector<QString> &texts)
    : Button(""), texts_(std::move(texts))
{
    if (texts_.empty())
        return;

    setText(texts.at(0));
}

void StateButton::updateState()
{
    auto current = text();
    auto begin = texts_.begin();
    auto end = texts_.end();

    for (auto iter = begin; iter != end; ++iter) {
        if (*iter != current)
            continue;

        auto next = iter + 1;

        if (next == end)
            next = begin;

        setText(*next);
    }
}

ButtonBox::ButtonBox(const std::vector<ButtonInfo> &list) : QDialogButtonBox()
{
    for (const auto &info : list) {
        auto text = getIndexedContainerValue(info.texts(), 0, DEFAULT_BUTTON_TEXT);
        auto button = addButton(text, info.role());

        connect(button, &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

ButtonBox::~ButtonBox() { logArgsInfo(Q_FUNC_INFO); }

ButtonPanelWidget::ButtonPanelWidget(WidgetType type, const std::vector<ButtonInfo> &list)
    : HLayoutWidget(type, Qt::AlignLeft), buttons_()
{
    for (const auto &info : list) {
        std::shared_ptr<Button> button = nullptr;

        auto texts = info.texts();
        auto size = texts.size();

        if (size > 1) {
            stateButton_ = std::make_shared<StateButton>(texts);
            button = stateButton_;
        } else if (size == 1) {
            button = std::make_shared<Button>(texts.at(0));
            buttons_.push_back(button);
        } else {
            continue;
        }

        addWidget(button.get());

        connect(button.get(), &QPushButton::clicked, this, info.function(), Qt::QueuedConnection);
    }
}

void ButtonPanelWidget::updateButtonState()
{
    stateButton_->updateState();
}
