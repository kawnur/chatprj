#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QMainWindow>
#include <thread>

#include "mainwindow.hpp"
#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"
#include "utils_gpgme.hpp"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
//    QMainWindow mainWindow;
    MainWindow* mainWindow = MainWindow::instance();

//    coutWithEndl("Test1");

//    coutWithEndl("library paths:");
//    for(QString& path : QCoreApplication::libraryPaths()) {
//        coutWithEndl(path.toStdString());
//    }

//    coutWithEndl("plugin paths:");
//    coutWithEndl(QLibraryInfo::path(QLibraryInfo::PluginsPath).toStdString());

//    mainWindow.show();
    mainWindow->show();

//    run_asio_tutorials();

    return a.exec();
}
