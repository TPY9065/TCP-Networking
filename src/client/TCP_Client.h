#pragma once
#include <string_view>
#include <array>
#include "core.hpp"
#include "connection/Connection.h"

using asio::ip::tcp;

namespace net
{
	// It is a template client class that create tcp connection with server. 
	// Protocal is the common communication rules between server and clients.
	template<Protocal T>
	class TcpClient
	{
	public:
		// Default constructor
		TcpClient();
		// Create a connection between client and server and keep the connection as a member variable 
		// and perform an async connection to the server, returns true if connection success, otherwise false.
		// This function should not be overriden.
		bool Connect(const std::string_view& host, const std::string_view& port);
		// Disconnect the connection between server and client. This function will be
		// called automatically when the connection failed when calling Connect().
		// This function should not be overriden.
		void Disconnect();
	protected:
		std::shared_ptr<Connection<T>> m_connection;
		MessageQueue<T> m_messages_queue;
	private:
		asio::io_context m_io_context;
		std::thread m_thread;
	};

	template<Protocal T>
	TcpClient<T>::TcpClient() : m_io_context(), m_messages_queue()
	{

	}

	template<Protocal T>
	bool TcpClient<T>::Connect(const std::string_view& host, const std::string_view& port)
	{
		try
		{
			tcp::resolver resolver(m_io_context);
			tcp::resolver::results_type endpoints = resolver.resolve(host, port);
			m_connection = std::make_shared<Connection<T>>(1000, m_io_context, tcp::socket(m_io_context), m_messages_queue);
			m_connection->ConnectToServer(endpoints);
			m_thread = std::thread([this]() { m_io_context.run(); });
			return true;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			Disconnect();
			return false;
		}
	}

	template<Protocal T>
	void TcpClient<T>::Disconnect()
	{
		m_connection->Disconnect();
		if (m_thread.joinable())
			m_thread.join();
	}
}