#include "mainwindow.hpp"

MainWindow* getMainWindowPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->mainWindowPtr_;
}

void MainWindow::addStubWidgetToLeftPanel()
{
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    this->leftPanelLayoutPtr_->addWidget(baseObjectCastPtr);
}

//void MainWindow::testMainWindowRightPanel() {
//    int j = 0;

//    while(true) {
//        coutWithEndl(j);
//        logLine(std::to_string(j));
//        j++;
//        sleep(.1);
////        std::this_thread::sleep_for(std::chrono::milliseconds(200));
//    }
//}

//void MainWindow::addTestSocketInfoWidgetToLeftPanel() {
//    SocketInfo* test1 = new SocketInfo("test1", "test1");
//    SocketInfo* test2 = new SocketInfo("test2", "test2");

//    this->addSocketInfoWidgetToLeftPanel(test1);
//    this->addSocketInfoWidgetToLeftPanel(test2);
//}

//MainWindow* MainWindow::_instance = nullptr;

//MainWindow* MainWindow::instance() {
//    if(!_instance) {
//        _instance = new MainWindow();
//    }
//    return _instance;
//}

SocketInfoBaseWidget* MainWindow::getMappedSocketInfoBaseWidgetByCompanion(
        const Companion* companion) const
{
    return this->map_.at(companion)->socketInfoBasePtr_;
}

const Companion* MainWindow::getMappedCompanionBySocketInfoBaseWidget(
        SocketInfoBaseWidget* widget) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->socketInfoBasePtr_ == widget;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

const Companion* MainWindow::getMappedCompanionByTextEditWidget(
        TextEditWidget* widget) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second->textEditPtr_ == widget;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

const Companion* MainWindow::getMappedCompanionByWidgetGroup(
        WidgetGroup* group) const
{
    auto findWidget = [&](const std::pair<const Companion*, WidgetGroup*> pair){
        return pair.second == group;
    };

    auto result = std::find_if(
                this->map_.cbegin(), this->map_.cend(), findWidget);

    return result->first;
}

void MainWindow::resetSelectedCompanion(const Companion* newSelected)
{  // TODO move to manager?
//    logArgs("this->selectedCompanion_:", this->selectedCompanion_);
//    logArgs("newSelected:", newSelected);

    if(this->selectedCompanion_ != nullptr)
    {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->unselect();

        this->companionNameLabelPtr_->setText("");
//        this->companionNameLabel_->hide();

        this->chatHistoryWidgetStubPtr_->setPlainText("");

        widgetGroup->chatHistoryPtr_->hide();
        widgetGroup->textEditPtr_->hide();
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->hide();
        this->textEditStubPtr_->hide();
    }

    this->selectedCompanion_ = newSelected;

    if(newSelected != nullptr)
    {
        auto widgetGroup = this->map_.at(this->selectedCompanion_);

        dynamic_cast<SocketInfoWidget*>(widgetGroup->socketInfoBasePtr_)->select();

        this->companionNameLabelPtr_->setText(
            QString::fromStdString(this->selectedCompanion_->getName()));
        this->companionNameLabelPtr_->show();

        widgetGroup->chatHistoryPtr_->show();
        widgetGroup->textEditPtr_->show();
    }
    else
    {
        this->chatHistoryWidgetStubPtr_->setPlainText("");
        this->chatHistoryWidgetStubPtr_->show();

        this->textEditStubPtr_->setText("");
        this->textEditStubPtr_->show();
    }
}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
MainWindow::MainWindow()
{
    setWindowTitle(std::getenv("CLIENT_NAME"));

    centralWidgetPtr_ = new QWidget;
    setCentralWidget(centralWidgetPtr_);

    centralWidgetLayoutPtr_ = new QHBoxLayout;
    centralWidgetLayoutPtr_->setSpacing(0);
    centralWidgetLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    centralWidgetPtr_->setLayout(centralWidgetLayoutPtr_);

    // left panel

    leftPanelPtr_ = new QWidget(centralWidgetPtr_);
    // leftPanelPtr_->setStyleSheet("border-right: 1px solid black");
    leftPanelPtr_->resize(2000, 1000);  // TODO ???
    leftPanelLayoutPtr_ = new QVBoxLayout(leftPanelPtr_);
    leftPanelLayoutPtr_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    leftPanelLayoutPtr_->setSpacing(0);
    leftPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    leftPanelPtr_->setLayout(leftPanelLayoutPtr_);

    centralWidgetLayoutPtr_->addWidget(leftPanelPtr_);

    // add stub widget to left panel
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
        dynamic_cast<SocketInfoBaseWidget*>(stub);

    leftPanelLayoutPtr_->addWidget(baseObjectCastPtr);

    leftPanelPtr_->resize(4000, 1000);  // TODO ???

    // central panel

    centralPanelPtr_ = new QWidget(centralWidgetPtr_);
    // centralPanelPtr_->setStyleSheet("border-right: 1px solid black");
    centralPanelLayoutPtr_ = new QVBoxLayout;
    centralPanelLayoutPtr_->setSpacing(0);
    centralPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    centralPanelPtr_->setLayout(centralPanelLayoutPtr_);

    companionNameLabelPtr_ = new QLabel("", centralPanelPtr_);
    // companionNameLabelPtr_->setStyleSheet("border-bottom: 1px solid black");
    companionNameLabelPalettePtr_ = new QPalette;
    companionNameLabelPalettePtr_->setColor(QPalette::Window, QColor(0xa9a9a9));
    companionNameLabelPtr_->setAutoFillBackground(true);
    companionNameLabelPtr_->setPalette(*companionNameLabelPalettePtr_);
    //    companionNameLabel_->hide();

    centralPanelLayoutPtr_->addWidget(companionNameLabelPtr_);

    chatHistoryWidgetStubPtr_ = new QPlainTextEdit(centralPanelPtr_);
    chatHistoryWidgetStubPtr_->setReadOnly(true);
    chatHistoryWidgetStubPtr_->setPlainText("");

    chatHistoryWidgetStubPalettePtr_ = new QPalette;
    chatHistoryWidgetStubPalettePtr_->setColor(QPalette::Base, QColorConstants::LightGray);
    //    chatHistoryWidgetStubPalette_->setColor(QPalette::Text, QColorConstants::Black);
    //    chatHistoryWidgetStub_->setAutoFillBackground(true);
    chatHistoryWidgetStubPtr_->setPalette(*chatHistoryWidgetStubPalettePtr_);

    centralPanelLayoutPtr_->addWidget(chatHistoryWidgetStubPtr_);

    textEditStubPtr_ = new TextEditWidget;
    centralPanelLayoutPtr_->addWidget(textEditStubPtr_);
    //    textEdit_->hide();

    centralWidgetLayoutPtr_->addWidget(centralPanelPtr_);

    // right panel

    rightPanelPtr_ = new QWidget(centralWidgetPtr_);
    // rightPanelPtr_->setStyleSheet("border-right: 1px solid black");
    rightPanelLayoutPtr_ = new QVBoxLayout;
    rightPanelLayoutPtr_->setSpacing(0);
    rightPanelLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    rightPanelPtr_->setLayout(rightPanelLayoutPtr_);

    appLogWidgetPtr_ = new QPlainTextEdit(centralWidgetPtr_);
    // appLogWidgetPtr_->setStyleSheet("border-left: 1px solid black");
    appLogWidgetPtr_->setReadOnly(true);
    appLogWidgetPtr_->setPlainText("");

    appLogWidgetPalettePtr_ = new QPalette;
    appLogWidgetPalettePtr_->setColor(QPalette::Base, QColor(0xcccaca));
    appLogWidgetPalettePtr_->setColor(QPalette::Text, QColorConstants::Black);
    appLogWidgetPtr_->setAutoFillBackground(true);
    appLogWidgetPtr_->setPalette(*appLogWidgetPalettePtr_);

    rightPanelLayoutPtr_->addWidget(appLogWidgetPtr_);

    //    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
    //    connect(
    //                testPlainTextEditButton_,
    //                &QPushButton::clicked,
    //                this,
    //                &MainWindow::testMainWindowRightPanel);
    //    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    centralWidgetLayoutPtr_->addWidget(rightPanelPtr_);

    //    selectedSocketInfoWidget_ = nullptr;
    selectedCompanion_ = nullptr;

    //    map_ = std::map<SocketInfoBaseWidget*, const Companion*>();
    //    map_ = std::map<const Companion*, SocketInfoBaseWidget*>();
    map_ = std::map<const Companion*, WidgetGroup*>();

    // logging enabled, actions
    // setLeftPanel();
    //    addTestSocketInfoWidgetToLeftPanel();
}

MainWindow::~MainWindow()
{
    // cannot set parent for palette
    delete this->companionNameLabelPalettePtr_;
    delete this->chatHistoryWidgetStubPalettePtr_;
    delete this->appLogWidgetPalettePtr_;
}

void MainWindow::setLeftPanel()
{
    QList<SocketInfoBaseWidget*> leftPanelChildren =
        this->leftPanelPtr_->findChildren<SocketInfoBaseWidget*>(Qt::FindDirectChildrenOnly);

    if(leftPanelChildren.size() == 0)
    {
        this->addStubWidgetToLeftPanel();
    }
    else
    {
        logArgsWarning(
            "leftPanelChildren.size() != 0 "
            "at MainWindow::setLeftPanel()");
    }
}

void MainWindow::set()
{
    this->setLeftPanel();
}

void MainWindow::buildWidgetGroups(std::vector<Companion*>* companionsPtr)
{
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanelPtr_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    auto companionsSize = companionsPtr->size();
    auto childrenSize = leftPanelChildren.size();

    logArgs("companionsSize:", companionsSize);
    logArgs("childrenSize:", childrenSize);

    for(auto& child : leftPanelChildren)
    {
        logArgs("child:", child);

        if(child->isStub())
        {
            logArgs("child->isStub() == true");
        }
    }

    if(companionsSize == 0 && childrenSize == 0)
    {
        logArgsWarning("strange case, empty sockets panel");
        this->addStubWidgetToLeftPanel();
    }
    else
    {
        if(childrenSize != 0)
        {
            // TODO check if sockets already are children

            for(auto& child : leftPanelChildren)
            {
                if(child->isStub())
                {
                    this->leftPanelLayoutPtr_->removeWidget(child);
                    delete child;
                }                
            }

            for(auto& companion : *companionsPtr)
            {
                WidgetGroup* widgetGroup = new WidgetGroup(companion);
                this->map_[companion] = widgetGroup;
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::exit(0);
}

void MainWindow::addTextToChatHistoryWidget(const QString& text)
{
    this->chatHistoryWidgetStubPtr_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addTextToAppLogWidget(const QString& text)
{
    this->appLogWidgetPtr_->appendPlainText(text);
    QApplication::processEvents();
}

void MainWindow::addWidgetToLeftPanel(SocketInfoBaseWidget* widget)
{
    this->leftPanelLayoutPtr_->addWidget(widget);
}

void MainWindow::addWidgetToCentralPanel(QWidget* widget)
{
    this->centralPanelLayoutPtr_->addWidget(widget);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->resetSelectedCompanion(nullptr);
    }
}
