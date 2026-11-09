#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <cstdint>
#include <memory>
// #include <mutex>

#include <QColor>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTextEdit>

#include "alias.hpp"
#include "logging.hpp"
// #include "message.hpp"

class Palette
{
public:
    Palette(WidgetType type, QPalette::ColorRole role);
    ~Palette() = default;

    WidgetType type() const;

    void setType(WidgetType type);
    void setColor(uint32_t color);

protected:
    WidgetType type_;
    QPalette::ColorRole role_;
    std::unique_ptr<QPalette> palette_;
};

class PalettedWidget : public QWidget, public Palette
{
    Q_OBJECT

public:
    PalettedWidget(WidgetType type);
    virtual ~PalettedWidget() = default;

    void updateColor(uint32_t color);
    void updateType(WidgetType type);
};

class Layout
{
public:
    Layout(WidgetType type);
    virtual ~Layout() = default;

    virtual void addWidget(QWidget *widget) {}
    virtual void removeWidget(QWidget *widget) {}

protected:
    WidgetType type_;
    uint32_t spacing_;
    uint32_t leftMargin_;
    uint32_t topMargin_;
    uint32_t rightMargin_;
    uint32_t bottomMargin_;
};

class VLayout : public Layout
{
public:
    VLayout(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~VLayout() = default;

    void addWidget(QWidget *widget) override;
    void removeWidget(QWidget *widget) override;
    void logLayout();

protected:
    std::unique_ptr<QVBoxLayout> layout_;
};

class HLayout : public Layout
{
public:
    HLayout(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~HLayout() = default;

    void addWidget(QWidget *widget) override;
    void removeWidget(QWidget *widget) override;
    void logLayout();

protected:
    std::unique_ptr<QHBoxLayout> layout_;
};

class VLayoutWidget : public PalettedWidget, public VLayout
{
    Q_OBJECT

public:
    VLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~VLayoutWidget() = default;
};

class HLayoutWidget : public PalettedWidget, public HLayout
{
    Q_OBJECT

public:
    HLayoutWidget(WidgetType type, Qt::AlignmentFlag alignment);
    virtual ~HLayoutWidget() = default;
};

class LabelWidget : public QLabel, public Palette
{
    Q_OBJECT

public:
    LabelWidget(WidgetType type, const QString &text);
    ~LabelWidget() = default;
};

class SplitterWidget : public QSplitter, public Palette
{
    Q_OBJECT

public:
    SplitterWidget(WidgetType type, QWidget *parent = nullptr);
    ~SplitterWidget() = default;
};

class TextEditWidget : public QTextEdit, public Palette
{
    Q_OBJECT

public:
    TextEditWidget(WidgetType type);
    ~TextEditWidget() = default;
};

class Button : public QPushButton
{
    Q_OBJECT

public:
    Button(ButtonType type);
    ~Button();

    virtual void updateState() {}

protected:
    ButtonType type_;
};

class StateButton : public Button
{
    Q_OBJECT

public:
    StateButton(ButtonType type);
    ~StateButton() = default;

    void updateState() override;

private:
    ButtonState state_;
};

class ButtonInfo
{
public:
    ButtonInfo();
    ButtonInfo(bool isStateButton, ButtonType type, std::function<void()> function);
    ~ButtonInfo();

    bool isStateButton() const;
    ButtonType type() const;
    std::function<void()> function() const;

private:
    bool isStateButton_;
    ButtonType type_;
    std::function<void()> function_;
};

class ButtonBox :public QDialogButtonBox
{
    Q_OBJECT

public:
    ButtonBox(const std::initializer_list<ButtonInfo> &list);
    ~ButtonBox();

private:
    // std::vector<Button> buttons;

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

class IndicatorWidget : public PalettedWidget
{
    Q_OBJECT

public:
    IndicatorWidget(WidgetType type, IndicatorWidgetState state);
    ~IndicatorWidget() = default;

    void setOn();
    void setOff();
    void updateState();

private:
    IndicatorWidgetState state_;
    uint8_t size_;
};

class ButtonPanelWidget : public HLayoutWidget
{
public:
    ButtonPanelWidget(WidgetType type, const std::initializer_list<ButtonInfo> &list);
    ~ButtonPanelWidget() = default;

    void updateButtonState(ButtonType type);

private:
    std::unordered_map<ButtonType, std::shared_ptr<Button>> buttons_;
};

class SocketInfoBaseWidget : public HLayoutWidget
{
    Q_OBJECT

public:
    SocketInfoBaseWidget();
    virtual ~SocketInfoBaseWidget() {}

    virtual bool isStub() { return false; }
    virtual void select() {}
    virtual void unselect() {}
    virtual void setNewMessagesIndicatorOn() {}
    virtual void setNewMessagesIndicatorOff() {}
    // virtual void updateNewMessagesIndicator() {}
    virtual void updateFields() {}
    virtual bool isConnected() { return false; }
    virtual void updateConnectionState() {}
    virtual CompanionPtr companion() { return nullptr; }

    void initializeFields();  // non-virtual because is called from constructor
};

class Menu : public QMenu
{
public:
    Menu() = default;
    ~Menu() { logArgsInfo(Q_FUNC_INFO); }
};

class SocketInfoWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoWidget(CompanionPtr companion);
    ~SocketInfoWidget() = default;

    void set(WidgetGroupPtr group);

    QString name() const;
    QString ipAddress() const;
    QString serverPort() const;
    QString clientPort() const;

    bool isStub() override;
    bool isSelected();
    bool isConnected() override;
    void select() override;
    void unselect() override;
    void updateFields() override;
    void setNewMessagesIndicatorOn() override;
    void setNewMessagesIndicatorOff() override;
    // void updateNewMessagesIndicator() override;

    void updateConnectionState() override;

    CompanionPtr companion() override;

public slots:
    void updateCompanionAction();
    void clearHistoryAction();
    void deleteCompanionAction();

private:
    void setValues();
    void setFieldsFromCompanionData();
    void mousePressEvent(QMouseEvent *event) override;

    bool isSelected_;
    bool isConnected_;
    CompanionPtr companion_;
    std::shared_ptr<IndicatorWidget> state_;
    std::shared_ptr<QLabel> name_;
    std::shared_ptr<QLabel> ipAddress_;
    std::shared_ptr<QLabel> serverPort_;
    std::shared_ptr<QLabel> clientPort_;
    ButtonType connectButtonType_;
    std::shared_ptr<ButtonPanelWidget> buttons_;
    std::shared_ptr<IndicatorWidget> received_;
    std::shared_ptr<QAction> requestHistory_;

private slots:
    void customMenuRequestedSlot(QPoint position);
};

class SocketInfoStubWidget : public SocketInfoBaseWidget
{
    Q_OBJECT

public:
    SocketInfoStubWidget();
    ~SocketInfoStubWidget() = default;

    bool isStub() override;

private:
    QString mark_;
    std::unique_ptr<QLabel> markLabel_;

    void initializeFields();
};

class ShowHideWidget : public VLayoutWidget
{
    Q_OBJECT

public:
    ShowHideWidget();
    ~ShowHideWidget() = default;

private:
    void hideInfo();
    void showInfo();
    void mousePressEvent(QMouseEvent *event) override;

    bool show_;
    std::unique_ptr<QLabel> label_;
};

class ScrollAreaWidget : public QScrollArea
{
    Q_OBJECT

public:
    ScrollAreaWidget(QWidgetPtr widget);
    ~ScrollAreaWidget() = default;
};

// class WidgetGroup : public QObject, public std::enable_shared_from_this<WidgetGroup>
// {
//     Q_OBJECT

// public:
//     WidgetGroup(CompanionPtr companion);
//     ~WidgetGroup();

//     void set();
//     void clearHistory();
//     void hideFields();
//     void hideCentralPanel();
//     void showCentralPanel();
//     void messageAdded();
//     void updateSocketInfoWidget();
//     void select();
//     void unselect();
//     void setNewMessagesIndicatorOn();
//     void setNewMessagesIndicatorOff();

// signals:
//     void addTextMessageWidgetSignal(MessagePtr message);
//     void addFileMessageWidgetSignal(MessagePtr message);
//     void addMessageWidgetsSignal(const std::vector<MessagePtr> &messages);  // TODO rename
//     // void askUserForHistorySendingConfirmationSignal();
//     // void buildChatHistorySignal();

// public slots:
//     void messageWidgetSelected(MessageWidgetPtr widget);
//     // void buildChatHistorySlot();
//     void requestHistorySlot();
//     void clientAction();

// private slots:
//     void addTextMessageWidgetSlot(MessagePtr message);
//     void addFileMessageWidgetSlot(MessagePtr message);
//     void addMessageWidgetsSlot(const std::vector<MessagePtr> &messages);
//     // void askUserForHistorySendingConfirmationSlot();

// private:
//     template<typename T>
//     void addMessageWidget(MessagePtr message)
//     {
//         bool isAntecedent = message->isAntecedent();

//         if (isAntecedent) {
//             std::lock_guard<std::mutex> lock(antecedentMessagesCounterMutex_);

//             ++antecedentMessagesCounter_;

//             logTemplateInfo("antecedentMessagesCounter_: {}", antecedentMessagesCounter_);
//         }

//         // auto widget = getGraphicManager()->buildMessageWidget(companion_, message);
//         auto widget = std::make_shared<T>(companion_, message);

//         // auto lambda = [=]()
//         // {
//         //     // companion->setMappedMessageWidget(message, widget);
//         //     getManager()->setMappedMessageWidget(companion_, message, widget);
//         // };

//         // runInDetachedThread(lambda);
//         auto result = getGraphicManager()->setMappedMessageWidget(companion_, message, widget);
//         // widget->setBase(shared_from_this());
//         widget->set(shared_from_this());
//         centralPanel_->addMessage(widget, isAntecedent);
//         messageAdded();
//     }

//     CompanionPtr companion_;
//     SocketInfoBaseWidgetPtr socketInfoBase_;
//     std::shared_ptr<CentralPanelWidget> centralPanel_;
//     uint32_t antecedentMessagesCounter_;
//     std::mutex antecedentMessagesCounterMutex_;
// };

class StubWidgetGroup
{
public:
    StubWidgetGroup();
    ~StubWidgetGroup() = default;

    void hide();
    void show();
    void hideSocketInfo();
    void hideCentral();
    void showCentral();
    void setLeftPanelWidth(int width);
    std::shared_ptr<SocketInfoBaseWidget> socketInfoBase() const;
    std::shared_ptr<LeftPanelWidget> left() const;
    std::shared_ptr<CentralPanelWidget> central() const;
    std::shared_ptr<RightPanelWidget> right() const;

private:
    std::shared_ptr<SocketInfoStubWidget> socketInfo_;
    std::shared_ptr<LeftPanelWidget> left_;
    std::shared_ptr<CentralPanelWidget> central_;
    std::shared_ptr<RightPanelWidget> right_;
};

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object);

#endif // WIDGETS_HPP
