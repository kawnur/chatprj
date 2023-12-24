#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRectF>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>
#include <thread>

#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    static MainWindow* _instance;

    QWidget* centralWidget_;
    QVBoxLayout* centralWidgetLayout_;
    QGraphicsScene* graphicsScene_;
    QGraphicsView* graphicsView_;
    QRectF* rect_;
    QGraphicsSimpleTextItem* textItem_;
    int linesCount_;
    QFont* font_;
    QPushButton* button_;

    MainWindow(QWidget* parent = nullptr);
    void set();
    void closeEvent(QCloseEvent*);
    virtual ~MainWindow() = default;

public:
    static MainWindow* instance();

    void addText(const QString&);
};

#endif // MAINWINDOW_HPP
