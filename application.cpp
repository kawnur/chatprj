#include "application.hpp"

ChatApp::~ChatApp()
{
    delete this->graphicManagerPtr_;
    delete this->managerPtr_;
}

void ChatApp::set()
{
    // client_ = new ChatClient;

    // boost::asio::io_context* io_context_ = new boost::asio::io_context;
    // io_context_ = new boost::asio::io_context;

    // server_ = new ChatServer(*io_context_, 5002);

    graphicManagerPtr_ = new GraphicManager;
    graphicManagerPtr_->set();

    // mainWindowPtr_ = new MainWindow;
    // mainWindowPtr_->set();

    managerPtr_ = new Manager;
    managerPtr_->set();

    // mainWindowPtr_->show();

    // mainWindowPtr_->show();

//    testQString();  // TODO remove
}

// ChatClient* getChatClientPtr()
// {
//     QCoreApplication* coreApp = QCoreApplication::instance();
//     ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
//     return app->client_;
// }
