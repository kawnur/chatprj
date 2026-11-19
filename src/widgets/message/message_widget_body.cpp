#include "message_widget_body.hpp"

#include "graphic_manager.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "utils_widgets.hpp"

MessageBodyWidget::MessageBodyWidget() : HLayoutWidget(WidgetType::MESSAGE, Qt::AlignLeft) {}

TextMessageBodyWidget::TextMessageBodyWidget(WidgetType type, MessagePtr message)
    : MessageBodyWidget()
{
    setType(type);
    auto text = message->_data();
    auto color = getHTMLColor(type);
    auto body = getHTMLBody(color, text, true, false);
    label_ = std::make_shared<QLabel>(body);
    addWidget(label_.get());
}

IncomingTextMessageBodyWidget::IncomingTextMessageBodyWidget(MessagePtr message)
    : TextMessageBodyWidget(WidgetType::TEXT_RECEIVED, message) {}

OutgoingTextMessageBodyWidget::OutgoingTextMessageBodyWidget(MessagePtr message)
    : TextMessageBodyWidget(WidgetType::TEXT_SENT, message) {}

FileMessageBodyWidget::FileMessageBodyWidget(WidgetType type, MessagePtr message) : MessageBodyWidget()
{
    // type
    setType(type);

    // label
    label_ = std::make_shared<QLabel>();
    addWidget(label_.get());
}

void FileMessageBodyWidget::buildText(MessagePtr message)
{
    auto _type = type();
    auto color = getHTMLColor(_type);
    auto textHtml = getHTMLBody(color, getText(message), true, false);
    label_->setText(textHtml);
}

IncomingFileMessageBodyWidget::IncomingFileMessageBodyWidget(MessagePtr message)
    : FileMessageBodyWidget(WidgetType::FILE_RECEIVED, message)
{
    button_ = std::make_shared<QPushButton>(DOWNLOAD_FILE_BUTTON_TEXT);
    layout_->addWidget(button_.get());
    buildText(message);
}

std::shared_ptr<QPushButton> IncomingFileMessageBodyWidget::button() const
{
    return button_;
}

std::string IncomingFileMessageBodyWidget::getText(MessagePtr message)
{
    return getStringByFormat("RECEIVE FILE: {}", message->_data());
}

OutgoingFileMessageBodyWidget::OutgoingFileMessageBodyWidget(MessagePtr message)
    : FileMessageBodyWidget(WidgetType::FILE_SENT, message)
{
    buildText(message);
}

std::string OutgoingFileMessageBodyWidget::getText(MessagePtr message)
{
    auto path = getGraphicManager()->getFilePathString(message->networkId());

    return getStringByFormat("SEND FILE: {}", path);
}
