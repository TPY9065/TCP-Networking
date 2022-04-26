#pragma once
#include <string>
#include <vector>
#include "core.hpp"
#include "connection/Connection.h"
#include "connection/MessageQueue.h"

using asio::ip::tcp;

namespace net
{
	class TCP_Server
	{
	public:
		TCP_Server();
		void Run();
	private:
		void StartAccept();
		void HandleAccept(const asio::error_code& error, tcp::socket peer);
	private:
		asio::io_context m_io_context;
		tcp::acceptor m_acceptor;
		std::vector<Connection::ptr> m_connections;
	};
}

