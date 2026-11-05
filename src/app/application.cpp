#include "application.hpp"

#include "graphic_manager.hpp"
#include "manager.hpp"

ChatApp::ChatApp(int argc, char *argv[]) : QApplication(argc, argv)
{
    graphicManager_ = std::make_shared<GraphicManager>();
    manager_ = std::make_shared<Manager>(graphicManager_);

    graphicManager_->setManager(manager_);
    manager_->set();

    // manager_->startUserAuthentication();
}
