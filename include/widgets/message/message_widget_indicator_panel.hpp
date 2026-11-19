#ifndef INDICATOR_PANEL_HPP
#define INDICATOR_PANEL_HPP

#include <QLabel>

#include "alias.hpp"
#include "widget_layout.hpp"

class MessageIndicatorPanelWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageIndicatorPanelWidget();
    virtual ~MessageIndicatorPanelWidget() = default;
};

class IncomingTextMessageIndicatorPanelWidget : public MessageIndicatorPanelWidget
{
    Q_OBJECT

public:
    IncomingTextMessageIndicatorPanelWidget(
        IncomingTextMessagePtr message);

    ~IncomingTextMessageIndicatorPanelWidget() = default;

    void unsetNewMessageLabel();

private:
    std::shared_ptr<QLabel> newMessageLabel_;
};

class OutgoingTextMessageIndicatorPanelWidget : public MessageIndicatorPanelWidget
{
    Q_OBJECT

public:
    OutgoingTextMessageIndicatorPanelWidget(MessagePtr message);
    ~OutgoingTextMessageIndicatorPanelWidget() = default;

    void setSentIndicatorOn();
    void setReceivedIndicatorOn();

private:
    IndicatorWidgetPtr sent_;
    IndicatorWidgetPtr received_;
};

#endif // INDICATOR_PANEL_HPP
