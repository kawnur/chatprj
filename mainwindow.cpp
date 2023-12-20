#include "mainwindow.hpp"

MainWindow* MainWindow::_instance = nullptr;

MainWindow* MainWindow::instance() {
    if(_instance == nullptr) {
        _instance = new MainWindow();
//        _instance = new MainWindow;
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

    centralWidgetLayout_ = new QVBoxLayout();
    centralWidgetLayout_->setSpacing(0);
    centralWidgetLayout_->setContentsMargins(0, 0, 0, 0);

    centralWidget_->setLayout(centralWidgetLayout_);

//    text_ = new QPlainTextEdit(this);
//    centralWidgetLayout_->addWidget(text_);

    graphicsScene_ = new QGraphicsScene(this);

//    textItem_ = graphicsScene_->addText("test");

    graphicsSceneView_ = new QGraphicsView(graphicsScene_);
    graphicsSceneView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    rect_ = new QRectF();
    rect_->setHeight(400);
    rect_->setWidth(400);

    graphicsScene_->setSceneRect(*rect_);
//    graphicsSceneView_->setSceneRect(*rect_);

    centralWidgetLayout_->addWidget(graphicsSceneView_);

//    graphicsSceneView_->show();

    AsioTest* test = AsioTest::instance();

    button_ = new QPushButton("Run ASIO tests");
    connect(button_, &QPushButton::clicked, test, &AsioTest::runTest);

    centralWidgetLayout_->addWidget(button_);
}

void MainWindow::addText(const QString& text) {
    coutWithEndl("MainWindow::addText");

    this->graphicsScene_->clear();
    textItem_ = graphicsScene_->addText(text);
//    this->textItem_->setPlainText(text);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

//    this->graphicsSceneView_->show();
}
