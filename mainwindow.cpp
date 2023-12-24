#include "mainwindow.hpp"

MainWindow* MainWindow::_instance = nullptr;

MainWindow* MainWindow::instance() {
    if(_instance == nullptr) {
        _instance = new MainWindow();
    }
    return _instance;
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle(QString("MainWindow"));
    set();
}

void MainWindow::set() {
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    centralWidgetLayout_ = new QVBoxLayout(this);
    centralWidgetLayout_->setSpacing(0);
    centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);
    centralWidget_->setLayout(centralWidgetLayout_);

    graphicsScene_ = new QGraphicsScene(this);
    textItem_ = graphicsScene_->addSimpleText("");
    font_ = new QFont;
//    font_->setPointSize(10);
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
    rect_->setWidth(300);
    rect_->setHeight(300);
    graphicsScene_->setSceneRect(*rect_);
//    graphicsView_->setSceneRect(*rect_);
    centralWidgetLayout_->addWidget(graphicsView_);

    graphicsView_->viewport()->resize(400, 400);

    AsioTest* test = AsioTest::instance();
    button_ = new QPushButton("Run ASIO tests");
    connect(button_, &QPushButton::clicked, test, &AsioTest::runTest);
    centralWidgetLayout_->addWidget(button_);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    std::exit(0);
}

void MainWindow::addText(const QString& text) {
    auto currentText = this->textItem_->text();
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

    coutArgsWithSpaceSeparator("textHeight:", textHeight);

    auto viewportWidth = this->graphicsView_->viewport()->size().width();
    auto viewportHeight = this->graphicsView_->viewport()->size().height();

    coutArgsWithSpaceSeparator("viewportWidth:", viewportWidth);
    coutArgsWithSpaceSeparator("viewportHeight:", viewportHeight);

    if(textHeight > viewportHeight) {
        this->graphicsView_->viewport()->resize(viewportWidth, textHeight);

        this->graphicsView_->verticalScrollBar()->setPageStep(textHeight);
        this->graphicsView_->horizontalScrollBar()->setPageStep(viewportWidth);

        this->graphicsView_->verticalScrollBar()->setRange(0, textHeight - viewportHeight);
        this->graphicsView_->horizontalScrollBar()->setRange(0, viewportWidth);

        int hvalue = this->graphicsView_->horizontalScrollBar()->value();
        int vvalue = this->graphicsView_->verticalScrollBar()->value();
        QPoint topLeft = this->graphicsView_->viewport()->rect().topLeft();

        this->graphicsView_->viewport()->move(topLeft.x() - hvalue, topLeft.y() - vvalue);


    }

//    this->graphicsView_->ensureVisible(
//                0,
//                textSize - fontSize,
////                this->rect_->width(),
//                10,
//                fontSize,
//                0,
//                0);

//    this->graphicsView_->setSceneRect(
//                0,
//                textSize - fontSize,
//                10,
//                10);

//    coutArgsWithSpaceSeparator("horizontalScrollBarPolicy:", this->graphicsView_->horizontalScrollBarPolicy());
//    coutArgsWithSpaceSeparator("verticalScrollBarPolicy:", this->graphicsView_->verticalScrollBarPolicy());

//    coutArgsWithSpaceSeparator("rect topLeft x:", this->rect_->topLeft().x());
//    coutArgsWithSpaceSeparator("rect topLeft y:", this->rect_->topLeft().y());
//    coutArgsWithSpaceSeparator("rect bottomRight x:", this->rect_->bottomRight().x());
//    coutArgsWithSpaceSeparator("rect bottomRight y:", this->rect_->bottomRight().y());
//    coutArgsWithSpaceSeparator("rect width:", this->rect_->width());
//    coutArgsWithSpaceSeparator("rect height:", this->rect_->height());


    QApplication::processEvents();
}
