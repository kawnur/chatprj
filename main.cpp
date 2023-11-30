#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <QApplication>
#include <QMainWindow>

#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"
#include "utils_gpgme.hpp"


int main(int argc, char *argv[]) {
//    run_asio_tutorials();
    QApplication a(argc, argv);
    QMainWindow mainWindow;
//    MainWindow* mainWindow = MainWindow::instance();

    mainWindow.show();
    return a.exec();

    return 0;
}

