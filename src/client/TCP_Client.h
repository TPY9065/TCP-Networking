#pragma once
#include <string_view>
#include <array>
#include "core.hpp"
#include "connection/Connection.h"

using asio::ip::tcp;

namespace net
{
	class TCP_Client
	{
	public:
		TCP_Client();
		void Connect(const std::string_view& host, const std::string_view& port);
	private:
		asio::io_context m_io_context;
		Connection::ptr connection;
	};
}

