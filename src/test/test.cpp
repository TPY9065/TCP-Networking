#include <iostream>
#include "server/TCP_Server.h"

int main()
{
    try
    {
        asio::io_context io;
        net::TCP_Server server();
        io.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}