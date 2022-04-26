#pragma once
#include <string>
#include <string_view>
#include "core.hpp"
#include "MessageQueue.h"

using asio::ip::tcp;

namespace net
{
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		using ptr = std::shared_ptr<Connection>;
		Connection(asio::io_context& io_context, tcp::socket socket);
		void ConnectToServer(const tcp::resolver::results_type& endpoints);
		void ConnectToClient();
		void Start();
	private:
		void ConnectHandler(const asio::error_code& error, const tcp::endpoint& endpoint);
		void WriteMessage();
		void WriteHandler(const asio::error_code& error, size_t bytes_transferred);
		void ReadMessage();
		void ReadHandler(const asio::error_code& error, size_t bytes_transferred);
	private:
		asio::io_context& m_io_context;
		tcp::socket m_socket;
		char m_message[128];
	public:
		MessageQueue<std::string> m_message_queue;
	};
}

