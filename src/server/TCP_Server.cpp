#include "TCP_Server.h"

namespace net
{
	TCP_Server::TCP_Server() : m_io_context(), 
		m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), 6000))
	{
		std::cout << "[SERVER] Started!" << std::endl;
		StartAccept();
		m_io_context.run();
	}

	void TCP_Server::Run()
	{
		while (true)
		{
			for (Connection::ptr connection : m_connections)
			{
				if (!connection->m_message_queue.Empty())
				{
					std::string message = connection->m_message_queue.ReadMessage();
					std::cout << message << '\n';
				}
			}
		}
	}

	void TCP_Server::StartAccept()
	{
		m_acceptor.async_accept(std::bind(&TCP_Server::HandleAccept, this, std::placeholders::_1, std::placeholders::_2));
	}
	
	void TCP_Server::HandleAccept(const asio::error_code& error, tcp::socket peer)
	{
		if (!error)
		{
			std::cout << "There is a new connection " << std::endl;
			Connection::ptr connection = std::make_shared<Connection>(m_io_context, std::move(peer));
			m_connections.push_back(std::move(connection));
			m_connections.back()->Start();
		}

		StartAccept();
	}
}