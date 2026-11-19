#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>

#include <QLabel>

#include "widget_layout.hpp"

class MessageHeaderWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    MessageHeaderWidget();

    virtual ~MessageHeaderWidget() = default;

protected:
    void setLabel(
        const std::string &color, const std::string &sender, const std::string &receiver,
        const std::string &time);

    std::shared_ptr<QLabel> label_;
};

class IncomingMessageHeaderWidget : public MessageHeaderWidget
{
    Q_OBJECT

public:
    IncomingMessageHeaderWidget(CompanionPtr companion, MessagePtr message);
    ~IncomingMessageHeaderWidget() = default;
};

class OutgoingMessageHeaderWidget : public MessageHeaderWidget
{
    Q_OBJECT

public:
    OutgoingMessageHeaderWidget(CompanionPtr companion, MessagePtr message);
    ~OutgoingMessageHeaderWidget() = default;
};

#endif // HEADER_HPP
