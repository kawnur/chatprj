#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <queue>

#include <QApplication>

#include "chat_client.hpp"
#include "chat_server.hpp"
#include "constants.hpp"
#include "db_interaction.hpp"
#include "mainwindow.hpp"
#include "manager.hpp"

class GraphicManager;
class Manager;

class ChatApp : public QApplication {
public:
    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {}
    ~ChatApp();

    void set();

    std::shared_ptr<GraphicManager> graphicManagerPtr_;
    std::shared_ptr<Manager> managerPtr_;
};

#endif // APPLICATION_HPP
