#include "application.hpp"

ChatApp::~ChatApp()
{
    delete manager_;
}

void ChatApp::set()
{
    client_ = new ChatClient;

    // boost::asio::io_context* io_context_ = new boost::asio::io_context;
    io_context_ = new boost::asio::io_context;

    server_ = new ChatServer(*io_context_, 5002);

    mainWindow_ = new MainWindow;
    mainWindow_->set();

    manager_ = new Manager;
    manager_->set();

    mainWindow_->show();

//    testQString();  // TODO remove
}

ChatClient* getChatClientPtr()
{
    QCoreApplication* coreApp = QCoreApplication::instance();
    ChatApp* app = dynamic_cast<ChatApp*>(coreApp);
    return app->client_;
}
