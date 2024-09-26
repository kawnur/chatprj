#include "chat_client.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient(std::string&& ipaddress, std::string&& port) :
    ipaddress_(ipaddress), port_(port), io_context_(), socket_(io_context_), resolver_(io_context_)
{
//    MainWindow* mainWindow = MainWindow::instance();
//    MainWindow* mainWindow = getMainWindowPtr();
//    setParent(mainWindow);
//    mainWindow->addText("Echo client started");
//    mainWindow->addText("Enter message: ");
}

bool ChatClient::connect()
{
    try
    {
        logArgs("ChatClient connects to", this->ipaddress_, this->port_);

        // boost::asio::io_context io_context_;
        // tcp::socket socket_ { io_context_ };
        // tcp::resolver resolver_ { io_context_ };
        //        boost::asio::connect(s, resolver.resolve("localhost", "5002"));
        //        boost::asio::connect(s, resolver.resolve("0.0.0.0", "5002"));
        //        boost::asio::connect(s, resolver.resolve("172.21.0.3", "5002"));
        // boost::asio::connect(s, resolver.resolve("chatprj-server-1", "5002"));
        //        boost::asio::connect(s, resolver.resolve("host.docker.internal", "5002"));
        boost::asio::connect(this->socket_, this->resolver_.resolve(this->ipaddress_, this->port_));

        // mainWindow->addTextToChatHistoryWidget("s.is_open(): "
        //                     + QString::fromStdString(std::to_string(s.is_open())));
        logArgs("socket_.is_open():", socket_.is_open());
    }
    catch(std::exception& e)
    {
        logArgsException(e.what());
    }

    return 0;
}

int ChatClient::send(std::string text)
{
    // MainWindow* mainWindow = getMainWindowPtr();

    try
    {
        // char request[max_length] = { 0 };

        // mainWindow->addTextToChatHistoryWidget(QString("request: ") + QString::fromStdString(text));
        // logArgs("request:", text);

        // auto textStdString = text.toStdString();
        // size_t request_length = textStdString.size();
        // std::strcpy(request, textStdString.data());
        // std::strcpy(request, text.data());

        // boost::asio::write(s, boost::asio::buffer(request, request_length));
        // boost::asio::write(this->socket_, boost::asio::buffer(request, text.size()));
        boost::asio::write(this->socket_, boost::asio::buffer(text.data(), text.size()));

        // char reply[max_length] = { 0 };
        // // size_t reply_length = boost::asio::read(s,
        // //                                         boost::asio::buffer(reply, request_length));
        // size_t reply_length = boost::asio::read(
        //     this->socket_,
        //     boost::asio::buffer(reply, text.size()));

        // std::string str(reply, reply_length);
        // // mainWindow->addTextToChatHistoryWidget("Reply is: " + QString::fromStdString(str));
        // logArgs("reply:", str);
        // // mainWindow->addTextToChatHistoryWidget("Enter message: ");
        // logArgs("Enter message:");
    }

    catch(std::exception& e)
    {
        // mainWindow->addTextToChatHistoryWidget("Exception: " + QString(e.what()));
        logArgsException(e.what());
    }

    return 0;
}
