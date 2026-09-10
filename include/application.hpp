#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>

#include <QApplication>

class GraphicManager;
class Manager;

class ChatApp : public QApplication
{
public:
    ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {}
    ~ChatApp() = default;

    void set();

    std::shared_ptr<GraphicManager> graphicManager_;
    std::shared_ptr<Manager> manager_;
};

#endif // APPLICATION_HPP
