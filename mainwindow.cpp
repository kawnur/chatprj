#include "mainwindow.hpp"

TextEdit::TextEdit(MainWindow* parent) {
   setParent(parent);
}

void TextEdit::keyPressEvent(QKeyEvent* event) {
//    coutWithEndl("keyPressEvent");
//    coutArgsWithSpaceSeparator("event->type():", std::hex, event->type());
//    coutArgsWithSpaceSeparator("event->key():", std::hex, event->key());
//    coutArgsWithSpaceSeparator("event->modifiers():", std::hex, event->modifiers());
//    endline(1);

    if(event->type() == QEvent::KeyPress
            && event->key() == Qt::Key_Return) {
        if(event->modifiers() == Qt::NoModifier) {
//            coutWithEndl("enter event");
            MainWindow* mainWindow = MainWindow::instance();

            auto text = this->toPlainText();

            mainWindow->addText(text);
            this->setText("");

            auto client = mainWindow->getClient();
            client->send(text);
        }
        else if(event->modifiers() == Qt::ControlModifier) {
            QKeyEvent eventEmulated = QKeyEvent(
                        QEvent::KeyPress,
                        Qt::Key_Return,
                        Qt::NoModifier);

            QTextEdit::keyPressEvent(&eventEmulated);
        }
    }
    else {
        QTextEdit::keyPressEvent(event);
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
    set();
}

//void MainWindow::set(QString role) {
void MainWindow::set() {
//    this->setWindowTitle(role);
//    role_ = role;

    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    centralWidgetLayout_ = new QVBoxLayout(this);
    centralWidgetLayout_->setSpacing(0);
    centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    centralWidget_->setLayout(centralWidgetLayout_);

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
    centralWidgetLayout_->addWidget(graphicsView_);

    graphicsView_->viewport()->resize(100, 100);

    textEdit_ = new TextEdit(this);
    centralWidgetLayout_->addWidget(textEdit_);

//    AsioTest* test = AsioTest::instance();
//    test->setRole(role_);
//    test->runTestFunc(role_);

//    this->messages_ = std::queue<QString>();

    button_ = new QPushButton("Run ASIO tests");
//    connect(button_, &QPushButton::clicked, test, &AsioTest::runTest);
    centralWidgetLayout_->addWidget(button_);
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
