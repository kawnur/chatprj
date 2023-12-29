#include "chat_client.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient() {
    MainWindow* mainWindow = MainWindow::instance();
    setParent(mainWindow);
    mainWindow->addText("Echo client started");
    mainWindow->addText("Enter message: ");
}

int ChatClient::send(QString text) {
    MainWindow* mainWindow = MainWindow::instance();

    try{
        boost::asio::io_context io_context;
        tcp::socket s { io_context };
        tcp::resolver resolver(io_context);
//        boost::asio::connect(s, resolver.resolve("localhost", "5002"));
//        boost::asio::connect(s, resolver.resolve("0.0.0.0", "5002"));
//        boost::asio::connect(s, resolver.resolve("172.21.0.3", "5002"));
        boost::asio::connect(s, resolver.resolve("chatprj-server-1", "5002"));
//        boost::asio::connect(s, resolver.resolve("host.docker.internal", "5002"));
        mainWindow->addText("s.is_open(): "
                            + QString::fromStdString(std::to_string(s.is_open())));

        char request[max_length];

        mainWindow->addText(QString("request: ") + QString(text));

        auto textStdString = text.toStdString();
        size_t request_length = textStdString.size();
        std::strcpy(request, textStdString.data());

        boost::asio::write(s, boost::asio::buffer(request, request_length));

        char reply[max_length];
        size_t reply_length = boost::asio::read(s,
            boost::asio::buffer(reply, request_length));

        std::string str(reply, reply_length);
        mainWindow->addText("Reply is: " + QString::fromStdString(str));
        mainWindow->addText("Enter message: ");
    }

    catch(std::exception& e) {
        mainWindow->addText("Exception: " + QString(e.what()));
    }

    return 0;
}
