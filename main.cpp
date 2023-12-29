#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QMainWindow>
#include <thread>

#include "application.hpp"
#include "mainwindow.hpp"
#include "tutorial_boost_asio.hpp"
#include "utils_cout.hpp"
#include "utils_gpgme.hpp"


//int runApp(int argc, char *argv[], QString role) {
int runApp(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    QMainWindow mainWindow;
//    MainWindow* mainWindow = MainWindow::instance();
    ChatApp app(argc, argv);

//    coutWithEndl("Test1");

//    coutWithEndl("library paths:");
//    for(QString& path : QCoreApplication::libraryPaths()) {
//        coutWithEndl(path.toStdString());
//    }

//    coutWithEndl("plugin paths:");
//    coutWithEndl(QLibraryInfo::path(QLibraryInfo::PluginsPath).toStdString());

//    mainWindow.show();

//    mainWindow->setWindowTitle(role);
//    mainWindow->set(role);
//    mainWindow->show();

    //    run_asio_tutorials();

    return app.exec();
}


int main(int argc, char *argv[]) {    
//    QString role;

    if(argc == 2) {
//        if(std::strcmp(argv[1], "--server") == 0) {
//            role = "Server";
//        }
//        else if(std::strcmp(argv[1], "--client") == 0) {
//            role = "Client";
//        }
        if(std::strcmp(argv[1], "--no-exec") == 0) {
            std::exit(0);
        }

//        runApp(argc, argv, role);
        ChatApp app(argc, argv);
        return app.exec();
    }
}
