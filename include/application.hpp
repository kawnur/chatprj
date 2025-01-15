#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <queue>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "db_interaction.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"

// class ChatClient;
// class ChatServer;
// class MainWindow;
class GraphicManager;
class Manager;

class ChatApp : public QApplication
{
public:
//    std::queue<QString> messages_;

    // ChatClient* client_;

    // boost::asio::io_context* io_context_;
    // ChatServer* server_;

    GraphicManager* graphicManagerPtr_;
    Manager* managerPtr_;

    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {}
    void set();

    ~ChatApp();
};

#endif // APPLICATION_HPP
