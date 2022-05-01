#include "server/tcp_server.h"

enum class Protocal
{
    SERVER, CLIENT
};

class Server : public net::TcpServer<Protocal>
{
protected:
    virtual void OnClientConnect(ConnectionPtr& new_connection) override
    {
        net::TcpServer<Protocal>::OnClientConnect(new_connection);
    }

};

int main()
{
    Server server;
    server.Start();

    return 0;
}