#include "server/TCP_Server.h"

int main()
{
    try
    {
        net::TCP_Server server;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}