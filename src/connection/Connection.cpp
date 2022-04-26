#include "Connection.h"

namespace net
{
	Connection::Connection(asio::io_context& io_context, tcp::socket socket) : 
		m_io_context(io_context), m_socket(std::move(socket)), m_message()
	{

	}

	void Connection::ConnectToServer(const tcp::resolver::results_type& endpoints)
	{
		asio::async_connect(m_socket, endpoints,
			std::bind(&Connection::ConnectHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Connection::ConnectToClient()
	{

	}

	void Connection::Start()
	{
		ReadMessage();
	}

	void Connection::ConnectHandler(const asio::error_code& error, const tcp::endpoint& endpoint)
	{
		if (!error)
			Start();
	}

	void Connection::WriteMessage()
	{
		asio::async_write(m_socket, asio::buffer("Accepted"),
			std::bind(&Connection::WriteHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Connection::WriteHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		if (!error)
			std::cout << "Write complete\n";
	}

	void Connection::ReadMessage()
	{
		asio::async_read(m_socket, asio::buffer(m_message, 128),
			std::bind(&Connection::ReadHandler, this, std::placeholders::_1, std::placeholders::_2));
	}
	
	void Connection::ReadHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		std::cout << "ReadHandler" << std::endl;
		if (!error)
		{
			std::cout << "New message! \n";
		}
		else
		{
			std::cerr << error.message() << "\n";
		}
		ReadMessage();
	}
}