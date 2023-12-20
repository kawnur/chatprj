#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRectF>
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
//    QPlainTextEdit* text_;
    QGraphicsScene* graphicsScene_;
    QGraphicsView* graphicsSceneView_;
    QRectF* rect_;
    QGraphicsTextItem* textItem_;
    QPushButton* button_;

    MainWindow(QWidget* parent = nullptr);
    void set();
    virtual ~MainWindow() = default;

public:
    static MainWindow* instance();

    void addText(const QString&);
};

#endif // MAINWINDOW_HPP
