#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <queue>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "db_interaction.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"

class GraphicManager;
class Manager;

class ChatApp : public QApplication
{
public:
    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {}
    ~ChatApp();

    void set();

    GraphicManager* graphicManagerPtr_;
    Manager* managerPtr_;
};

#endif // APPLICATION_HPP
