#include "mainwindow.hpp"

MainWindow* getMainWindowPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->mainWindow_;
}

void MainWindow::setLeftPanel() {
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanel_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    if(leftPanelChildren.size() == 0) {
        this->addStubWidgetToLeftPanel();
    }
    else {
        logArgs(
                    "WARNING: leftPanelChildren.size() != 0 "
                    "at MainWindow::setLeftPanel()");
    }
}

void MainWindow::addStubWidgetToLeftPanel() {
    SocketInfoStubWidget* stub = new SocketInfoStubWidget;

    SocketInfoBaseWidget* baseObjectCastPtr =
            dynamic_cast<SocketInfoBaseWidget*>(stub);

    baseObjectCastPtr->setParent(this->leftPanel_);
    this->leftPanelLayout_->addWidget(baseObjectCastPtr);
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

//MainWindow* MainWindow::_instance = nullptr;

//MainWindow* MainWindow::instance() {
//    if(_instance == nullptr) {
//        _instance = new MainWindow();
//    }
//    return _instance;
//}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
MainWindow::MainWindow() {
//    setWindowTitle(QString("MainWindow"));
    setWindowTitle(std::getenv("CLIENT_NAME"));
}

void MainWindow::buildSocketInfoWidgets(std::vector<SocketInfo>* socketsPtr) {
    QList<SocketInfoBaseWidget*> leftPanelChildren =
            this->leftPanel_->findChildren<SocketInfoBaseWidget*>(
                Qt::FindDirectChildrenOnly);

    auto socketsSize = socketsPtr->size();
    auto childrenSize = leftPanelChildren.size();

    logArgs("socketsSize:", socketsSize);
    logArgs("childrenSize:", childrenSize);

    for(auto& i : *socketsPtr) {
        i.print();
    }

    for(auto& child : leftPanelChildren) {
        logArgs("child:", child);

        if(child->isStub()) {
            logArgs("child->isStub() == true");
        }
    }

    if(socketsSize == 0) {
        if(childrenSize == 0) {
            logArgs("WARNING: strange case, empty sockets panel");

            this->addStubWidgetToLeftPanel();
        }
    }
    else {
        if(childrenSize != 0) {
            // TODO check if sockets already are children

            for(auto& child : leftPanelChildren) {
                if(child->isStub()) {
                    this->leftPanelLayout_->removeWidget(child);
                    delete child;
                }
            }

            for(auto& i : *socketsPtr) {
                SocketInfoWidget* widget = new SocketInfoWidget(
                            i.getName(), i.getIpaddress(), i.getPort());

                SocketInfoBaseWidget* widgetBase =
                        dynamic_cast<SocketInfoBaseWidget*>(widget);

                widgetBase->setParent(this->leftPanel_);
                this->leftPanelLayout_->addWidget(widgetBase);
            }
        }
    }
}

void MainWindow::set() {
    this->centralWidget_ = new QWidget(this);
    this->setCentralWidget(centralWidget_);

    this->centralWidgetLayout_ = new QHBoxLayout(this);
    this->centralWidgetLayout_->setSpacing(0);
    this->centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    this->centralWidget_->setLayout(centralWidgetLayout_);

    // left panel

    this->leftPanel_ = new QWidget(this);
    this->leftPanel_->resize(2000, 1000);
    this->leftPanelLayout_ = new QVBoxLayout(this);
    this->leftPanelLayout_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->leftPanelLayout_->setSpacing(0);
    this->leftPanelLayout_->setContentsMargins(0, 0, 0, 0);
    this->leftPanel_->setLayout(leftPanelLayout_);

//    this->test_ = new QLineEdit;
//    this->leftPanelLayout_->addWidget(this->test_);

    this->addStubWidgetToLeftPanel();

    this->centralWidgetLayout_->addWidget(leftPanel_);

    // central panel

    centralPanel_ = new QWidget(this);    
    centralPanelLayout_ = new QVBoxLayout(this);
    centralPanelLayout_->setSpacing(0);
    centralPanelLayout_->setContentsMargins(0, 0, 0, 0);
    centralPanel_->setLayout(centralPanelLayout_);

    graphicsScene_ = new QGraphicsScene(this);
    textItem_ = graphicsScene_->addSimpleText("");
    font_ = new QFont;
    font_->setPixelSize(15);
    textItem_->setFont(*font_);

    linesCount_ = 0;

    graphicsView_ = new QGraphicsView(graphicsScene_);
//    graphicsView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    graphicsView_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    graphicsView_->setBackgroundBrush(QBrush(QColor(QColorConstants::LightGray)));
//    graphicsView_->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    graphicsView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    graphicsView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    rect_ = new QRectF();
    rect_->setWidth(500);
    rect_->setHeight(10000);
    graphicsScene_->setSceneRect(*rect_);
//    graphicsView_->setSceneRect(*rect_);
    centralPanelLayout_->addWidget(graphicsView_);

    graphicsView_->viewport()->resize(100, 100);

    textEdit_ = new TextEditWidget(this);
    centralPanelLayout_->addWidget(textEdit_);

    centralWidgetLayout_->addWidget(centralPanel_);

    // right panel

    rightPanel_ = new QWidget(this);
    this->leftPanel_->resize(4000, 1000);
    rightPanelLayout_ = new QVBoxLayout(this);
    rightPanelLayout_->setSpacing(0);
    rightPanelLayout_->setContentsMargins(0, 0, 0, 0);
    rightPanel_->setLayout(rightPanelLayout_);

    plainTextEdit_ = new QPlainTextEdit(this);
    plainTextEdit_->setReadOnly(true);
    plainTextEdit_->setPlainText("");

    plainTextEditPalette_ = new QPalette();
    plainTextEditPalette_->setColor(QPalette::Base, QColorConstants::LightGray);
    plainTextEditPalette_->setColor(QPalette::Text, QColorConstants::Black);
    plainTextEdit_->setAutoFillBackground(true);
    plainTextEdit_->setPalette(*plainTextEditPalette_);

    rightPanelLayout_->addWidget(plainTextEdit_);

//    testPlainTextEditButton_ = new QPushButton("testPlainTextEditButton");
//    connect(
//                testPlainTextEditButton_,
//                &QPushButton::clicked,
//                this,
//                &MainWindow::testMainWindowRightPanel);
//    rightPanelLayout_->addWidget(testPlainTextEditButton_);

    centralWidgetLayout_->addWidget(rightPanel_);

    // logging enabled, actions
//    this->setLeftPanel();
}

//int MainWindow::setClient(EchoClient* client) {
//    this->client_ = client;
//    return 0;
//}

//EchoClient*  MainWindow::getClient() {
//    return this->client_;
//}

void MainWindow::closeEvent(QCloseEvent *event) {
    std::exit(0);
}

MainWindow::~MainWindow() {
//    delete[] &sockets_;
}

void MainWindow::addTextToCentralPanel(const QString& text) {
    auto currentText = this->textItem_->text();

    logArgs(
                "currentText:",
                currentText.replace("\n", "NL").toStdString(),
                ", text:",
                text.toStdString());

    QString newText;

    if(currentText == "") {
        newText = text;
    }
    else {
        newText = this->textItem_->text() + QString("\n") + text;
//        newText = currentText + QString("\n") + text;
    }

    ++linesCount_;

    this->textItem_->setText(newText);

    QFontInfo fontInfo = QFontInfo(this->textItem_->font());
    int fontInfoPixelSize = fontInfo.pixelSize();

    QFontMetrics fontMetrics = QFontMetrics(this->textItem_->font());
    QRect boundingRect = fontMetrics.boundingRect(newText);
    int boundingRectHeight = boundingRect.height();

    auto textHeight = boundingRectHeight * linesCount_;

//    coutArgsWithSpaceSeparator("fontInfoPixelSize:", fontInfoPixelSize);
//    coutArgsWithSpaceSeparator("boundingRectHeight:", boundingRectHeight);
//    coutArgsWithSpaceSeparator("font pixelSize:", this->textItem_->font().pixelSize());
//    coutArgsWithSpaceSeparator("font pointSize:", this->textItem_->font().pointSize());

//    coutArgsWithSpaceSeparator("rect topLeft x:", this->rect_->topLeft().x());
//    coutArgsWithSpaceSeparator("rect topLeft y:", this->rect_->topLeft().y());
//    coutArgsWithSpaceSeparator("rect bottomRight x:", this->rect_->bottomRight().x());
//    coutArgsWithSpaceSeparator("rect bottomRight y:", this->rect_->bottomRight().y());
//    coutArgsWithSpaceSeparator("rect width:", this->rect_->width());
//    coutArgsWithSpaceSeparator("rect height:", this->rect_->height());

//    coutArgsWithSpaceSeparator("textHeight:", textHeight);

    auto viewportWidth = this->graphicsView_->viewport()->width();
    auto viewportHeight = this->graphicsView_->viewport()->height();

//    coutArgsWithSpaceSeparator("viewportWidth:", viewportWidth);
//    coutArgsWithSpaceSeparator("viewportHeight:", viewportHeight);

    auto verticalScrollBarWidth = this->graphicsView_->verticalScrollBar()->width();
    auto verticalScrollBarHeight = this->graphicsView_->verticalScrollBar()->height();
    auto horizontalScrollBarWidth = this->graphicsView_->horizontalScrollBar()->width();
    auto horizontalScrollBarHeight = this->graphicsView_->horizontalScrollBar()->height();

//    coutArgsWithSpaceSeparator("verticalScrollBarWidth:", verticalScrollBarWidth);
//    coutArgsWithSpaceSeparator("verticalScrollBarHeight:", verticalScrollBarHeight);
//    coutArgsWithSpaceSeparator("horizontalScrollBarWidth:", horizontalScrollBarWidth);
//    coutArgsWithSpaceSeparator("horizontalScrollBarHeight:", horizontalScrollBarHeight);

//    if(textHeight > viewportHeight) {
    if(textHeight > this->rect_->height()) {
        this->rect_->setHeight(textHeight);

//        this->graphicsView_->viewport()->resize(viewportWidth, textHeight);

//        this->graphicsView_->verticalScrollBar()->setPageStep(textHeight);
//        this->graphicsView_->horizontalScrollBar()->setPageStep(viewportWidth);

//        this->graphicsView_->verticalScrollBar()->setRange(0, textHeight - viewportHeight);
//        this->graphicsView_->horizontalScrollBar()->setRange(0, viewportWidth);

//        int hvalue = this->graphicsView_->horizontalScrollBar()->value();
//        int vvalue = this->graphicsView_->verticalScrollBar()->value();
//        QPoint topLeft = this->graphicsView_->viewport()->rect().topLeft();

//        this->graphicsView_->viewport()->move(topLeft.x() - hvalue, topLeft.y() - vvalue);
//        this->graphicsView_->viewport()->move(0, (-1) * boundingRectHeight);
    }

//    QApplication::processEvents();

    this->graphicsView_->ensureVisible(
                0,
//                textHeight - boundingRectHeight + horizontalScrollBarHeight,
                textHeight + linesCount_,
//                this->rect_->height() + linesCount_,
//                this->rect_->width(),
                10,
                boundingRectHeight,
                0,
                0);

//    this->graphicsView_->setSceneRect(
//                0,
//                textSize - fontSize,
//                10,
//                10);

//    coutArgsWithSpaceSeparator("horizontalScrollBarPolicy:", this->graphicsView_->horizontalScrollBarPolicy());
//    coutArgsWithSpaceSeparator("verticalScrollBarPolicy:", this->graphicsView_->verticalScrollBarPolicy());

    QApplication::processEvents();
}

void MainWindow::addTextToRightPanel(const QString& text) {
//    coutWithEndl("addTextToRightPanel");

//    auto currentText = this->plainTextEdit_->toPlainText();

//    coutArgsWithSpaceSeparator(
//                "currentText:",
//                currentText.replace("\n", "NL").toStdString(),
//                ", text:",
//                text.toStdString());

//    QString newText;

//    if(currentText == "") {
//        newText = text;
//    }
//    else {
//        newText = currentText + QString("\n") + text;
//    }

//    this->plainTextEdit_->setPlainText(newText);

    this->plainTextEdit_->appendPlainText(text);
//    this->plainTextEdit_->show();

    QApplication::processEvents();
}

//void MainWindow::addMessagesToView() {
//    while(this->messages_.size() > 0) {
//    while(this->messages_.size() > 0) {
//        this->addText(this->messages_.front());
//        this->messages_.pop();
//    }
//}

//void addTextFunction(const QString& text) {
//    MainWindow* mainWindow = MainWindow::instance();
//    mainWindow->addText(text);
//}

//void MainWindow::pushMessage(const QString& text) {
//    this->messages_.push(text);
//}

//void MainWindow::pushMessageAndAddText(const QString& text) {
////    this->messages_.push(text);

////    std::thread(addTextFunction, text).detach();
//    this->addText(text);
//}
