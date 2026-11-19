#ifndef WIDGET_TEXT_EDIT_HPP
#define WIDGET_TEXT_EDIT_HPP

#include <QTextEdit>

#include "widget_palette.hpp"

class TextEditWidget : public QTextEdit, public Palette
{
    Q_OBJECT

public:
    TextEditWidget(WidgetType type, bool readOnly = false);
    ~TextEditWidget() = default;
};

class MessageEditWidget : public TextEditWidget
{
    Q_OBJECT

public:
    MessageEditWidget(WidgetType type);
    ~MessageEditWidget() = default;

private:
    void keyPressEvent(QKeyEvent *event);

signals:
    void sendSignal(const QString &text);

public slots:
    void sendSlot();
};

#endif // WIDGET_TEXT_EDIT_HPP
