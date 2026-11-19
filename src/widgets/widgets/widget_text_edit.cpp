#include "widget_text_edit.hpp"

#include <QKeyEvent>

#include "utils_widgets.hpp"

TextEditWidget::TextEditWidget(WidgetType type, bool readOnly) : Palette(type, QPalette::Base)
{
    setColor(getWidgetColor(type));
    setPalette(*palette_);
    setAutoFillBackground(true);
    setReadOnly(readOnly);
}

MessageEditWidget::MessageEditWidget(WidgetType type) : TextEditWidget(type) {}

void MessageEditWidget::keyPressEvent(QKeyEvent *event)
{
    //    coutWithEndl("keyPressEvent");
    //    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
    //    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
    //    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
    //    endline(1);

    if (event->type() == QEvent::KeyPress && event->key() == Qt::Key_Return) {
        if (event->modifiers() == Qt::NoModifier) {
            sendSlot();
        }
        else if (event->modifiers() == Qt::ControlModifier) {
            QKeyEvent eventEmulated = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

void MessageEditWidget::sendSlot()
{
    auto text = toPlainText();
    setText("");
    emit sendSignal(text);
}
