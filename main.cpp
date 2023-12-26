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


int runApp(int argc, char *argv[], QString role) {
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

//    mainWindow->setWindowTitle(role);
    mainWindow->set(role);
    mainWindow->show();

    //    run_asio_tutorials();

    return a.exec();
}


int main(int argc, char *argv[]) {    
    QString role;

    if(argc == 2) {
        if(std::strcmp(argv[1], "--server") == 0) {
            role = "Server";
        }
        else if(std::strcmp(argv[1], "--client") == 0) {
            role = "Client";
        }

        runApp(argc, argv, role);
    }
}
