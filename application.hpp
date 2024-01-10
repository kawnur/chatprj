#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <queue>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "mainwindow.hpp"

class ChatClient;
class ChatServer;
class MainWindow;

class ChatApp : public QApplication {

public:
//    std::queue<QString> messages_;

    ChatClient* client_;

    boost::asio::io_context* io_context_;
    ChatServer* server_;

    MainWindow* mainWindow_;

//    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {
//        client_ = new ChatClient;
//        server_ = new ChatServer(5002);
//        mainWindow_ = new MainWindow;
//    }
    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {};
    void set();

    ~ChatApp() = default;
};

ChatClient* getChatClientPtr();

#endif // APPLICATION_HPP
