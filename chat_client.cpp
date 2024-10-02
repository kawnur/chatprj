#include "chat_client.hpp"

using boost::asio::ip::tcp;

ChatClient::ChatClient(std::string&& ipAddress, uint16_t&& port) :
    ipAddress_(ipAddress), port_(port), io_context_(), socket_(io_context_), resolver_(io_context_)
{
//    MainWindow* mainWindow = MainWindow::instance();
//    MainWindow* mainWindow = getMainWindowPtr();
//    setParent(mainWindow);
//    mainWindow->addText("Echo client started");
//    mainWindow->addText("Enter message: ");
}

bool ChatClient::connect()
{
    bool connected = false;

    auto connectLambda = [this](bool& value)
    {
        logArgs("ChatClient connects to", this->ipAddress_, this->port_);

        boost::asio::connect(
            this->socket_,
            this->resolver_.resolve(this->ipAddress_, std::to_string(this->port_))
        );

        logArgs("socket_.is_open():", this->socket_.is_open());

        value = true;  // TODO check connection state
    };

    runAndLogException(connectLambda, connected);

    return connected;
}

bool ChatClient::disconnect()
{
    bool disconnected = false;

    auto disconnectLambda = [this](bool& value)
    {
        logArgs("ChatClient disconnects from", this->ipAddress_, this->port_);

        // boost::asio::io_context io_context_;
        // tcp::socket socket_ { io_context_ };
        // tcp::resolver resolver_ { io_context_ };
        //        boost::asio::connect(s, resolver.resolve("localhost", "5002"));
        //        boost::asio::connect(s, resolver.resolve("0.0.0.0", "5002"));
        //        boost::asio::connect(s, resolver.resolve("172.21.0.3", "5002"));
        // boost::asio::connect(s, resolver.resolve("chatprj-server-1", "5002"));
        //        boost::asio::connect(s, resolver.resolve("host.docker.internal", "5002"));

        this->socket_.close();

        // mainWindow->addTextToChatHistoryWidget("s.is_open(): "
        //                     + QString::fromStdString(std::to_string(s.is_open())));


        logArgs("socket_.is_open():", socket_.is_open());

        value = true;  // TODO check connection state
    };

    runAndLogException(disconnectLambda, disconnected);

    return disconnected;
}

bool ChatClient::send(std::string text)
{
    bool sent = false;

    auto sendLambda = [&, this](bool& value)
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

        value = true;
    };

    runAndLogException(sendLambda, sent);

    return sent;
}
