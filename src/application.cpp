#include "application.hpp"

ChatApp::~ChatApp() {
    delete this->graphicManagerPtr_;
    delete this->managerPtr_;
}

void ChatApp::set() {
    graphicManagerPtr_ = new GraphicManager;
    graphicManagerPtr_->set();

    managerPtr_ = new Manager;
    managerPtr_->set();

    // managerPtr_->startUserAuthentication();
}
