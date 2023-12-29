#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <queue>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "mainwindow.hpp"

class ChatApp : public QApplication {
private:
//    std::queue<QString> messages_;

    ChatClient* client_;
    ChatServer* server_;
    MainWindow* mainWindow_;

public:
    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {
        client_ = new ChatClient;
        server_ = new ChatServer;
        mainWindow_ = new MainWindow;

    }

    ~ChatApp() = default;
};

#endif // APPLICATION_HPP
