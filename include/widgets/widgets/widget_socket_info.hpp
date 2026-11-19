#ifndef WIDGET_SOCKET_INFO_HPP
#define WIDGET_SOCKET_INFO_HPP

#include <QLabel>

#include "alias.hpp"
#include "widget_button.hpp"
#include "widget_layout.hpp"
#include "widget_menu.hpp"

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
    IndicatorWidgetPtr state_;
    std::shared_ptr<QLabel> name_;
    std::shared_ptr<QLabel> ipAddress_;
    std::shared_ptr<QLabel> serverPort_;
    std::shared_ptr<QLabel> clientPort_;
    std::shared_ptr<ButtonPanelWidget> buttons_;
    IndicatorWidgetPtr received_;
    std::unique_ptr<Menu> menu_;

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

void logSocketInfoWidget(std::shared_ptr<SocketInfoWidget> object);

#endif // WIDGET_SOCKET_INFO_HPP
