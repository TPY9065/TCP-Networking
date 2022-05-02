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
		net::Message<Protocal> message;
		message << (m_connection_count + m_id);

		new_connection->WriteMessage(message);
		new_connection->ReadMessage();

		std::vector<size_t> dead_connections_id;
		for (auto& [id, conn] : m_connections)
		{
			if (conn->IsOpen())
			{
				conn->WriteMessage(message);
			}
			else
			{
				conn->Disconnect();
				dead_connections_id.push_back(id);
			}
		}

		for (auto id : dead_connections_id)
		{
			m_connections[id].reset();
			m_connections.erase(id);
		}

		m_connections[m_connection_count + m_id] = std::move(new_connection);
		m_connection_count += 1;
		std::cout << "Connections = " << m_connections.size() << std::endl;
    }
};

int main()
{
    Server server;
    server.Start();

    return 0;
}