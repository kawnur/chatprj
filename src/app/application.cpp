#include "application.hpp"

#include "graphic_manager.hpp"
#include "manager.hpp"

ChatApp::ChatApp(int argc, char *argv[]) : QApplication(argc, argv) {}

void ChatApp::set()
{
    graphicManager_ = std::make_shared<GraphicManager>();
    graphicManager_->set();

    manager_ = std::make_shared<Manager>();
    manager_->set();

    // manager_->startUserAuthentication();
}
