#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <QApplication>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QMainWindow>
#include <thread>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "mainwindow.hpp"
#include "application.hpp"
#include "utils_cout.hpp"
#include "utils_gpgme.hpp"


int main(int argc, char *argv[]) {
    if(argc == 2) {
        if(std::strcmp(argv[1], "--no-exec") == 0) {
            std::exit(0);
        }

        ChatApp app(argc, argv);
        app.set();

        return app.exec();
    }
}
