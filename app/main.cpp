#include "application.hpp"

int main(int argc, char *argv[])
{
    // if (argc != 2 && std::strcmp(argv[1], "--no-exec") == 0)
    //     std::exit(0);

    ChatApp app(argc, argv);
    app.set();

    return app.exec();
}
