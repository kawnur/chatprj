#include "mainwindow.hpp"

MainWindow* getMainWindowPtr() {
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->mainWindow_;
}

void MainWindow::buildSockets() {  // TODO move to manager
    MainWindow* mainWindow = getMainWindowPtr();

    PGconn* dbConnection = getDBConnection();

    ConnStatusType status = PQstatus(dbConnection);
    coutArgsWithSpaceSeparator("status:", status);
    QString statusQString = QString::fromStdString(std::to_string((int)status));
    mainWindow->addText(QString("status: ") + statusQString);

    PGresult* result = getsSocketsInfo(dbConnection);

    std::vector<std::vector<QString>> data = {
        {"name1", "192.168.1.1", "7777"},
        {"name2", "192.168.1.2", "7778"},
        {"name3", "192.168.1.3", "7779"}
    };

    for(auto& i : data) {
        this->sockets_.emplace_back(i.at(0), i.at(1), i.at(2));
    }
}

void MainWindow::setLeftPanel() {}

void MainWindow::connectToDb() {
    this->buildSockets();

    for(auto& i : this->sockets_) {
        this->leftPanelLayout_->addWidget(&i);
    }
}


//MainWindow* MainWindow::_instance = nullptr;

//MainWindow* MainWindow::instance() {
//    if(_instance == nullptr) {
//        _instance = new MainWindow();
//    }
//    return _instance;
//}

//MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
MainWindow::MainWindow() {
    setWindowTitle(QString("MainWindow"));
}

//void MainWindow::set(QString role) {
void MainWindow::set() {
//    this->setWindowTitle(role);
//    role_ = role;

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

    this->sockets_ = std::vector<SocketInfoWidget>();

    this->setLeftPanel();

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

//    AsioTest* test = AsioTest::instance();
//    test->setRole(role_);
//    test->runTestFunc(role_);

//    this->messages_ = std::queue<QString>();

//    button_ = new QPushButton("Run ASIO tests");
//    connect(button_, &QPushButton::clicked, test, &AsioTest::runTest);
//    centralPanelLayout_->addWidget(button_);

    centralWidgetLayout_->addWidget(centralPanel_);
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
//    delete[] sockets_;
}

void MainWindow::addText(const QString& text) {
    auto currentText = this->textItem_->text();

    coutArgsWithSpaceSeparator(
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
