#include "tcp_client.h"

namespace net
{
	TCP_Client::TCP_Client() : m_io_context()
	{
		
	}

	void TCP_Client::Connect(const std::string_view& host, const std::string_view& port)
	{
		try
		{
			tcp::resolver resolver(m_io_context);
			tcp::resolver::results_type endpoints = resolver.resolve(host, port);
			connection = std::make_shared<Connection>(m_io_context, tcp::socket(m_io_context));
			connection->ConnectToServer(endpoints);
			m_io_context.run();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << "\n";
		}
	}
}