#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>

#include <QApplication>

#include "alias.hpp"

class GraphicManager;
class Manager;

class ChatApp : public QApplication
{
public:
    ChatApp(int argc, char *argv[]);
    ~ChatApp() = default;

    void set();

    GraphicManagerPtr graphicManager_;
    ManagerPtr manager_;
};

#endif // APPLICATION_HPP
