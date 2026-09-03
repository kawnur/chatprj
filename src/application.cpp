#include "application.hpp"

void ChatApp::set() {
    graphicManager_ = std::make_shared<GraphicManager>();
    graphicManager_->set();

    manager_ = std::make_shared<Manager>();
    manager_->set();

    // manager_->startUserAuthentication();
}
