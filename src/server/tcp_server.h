#pragma once
#include <string>
#include <unordered_map>
#include "core.hpp"
#include "connection/Connection.h"

using asio::ip::tcp;

namespace net
{	
	// It is a template server class that open a socket and accept new connection
	// asynchronously. Users can override the virtual function OnClientConnect()
	// to perform upcoming processing where there is a new connection request accepted.
	// Protocal is the common communication rules between server and clients.
	template<Protocal T>
	class TcpServer
	{
	public:
		// Default constructor
		TcpServer();
		// Start to listen the connection request asynchronously. 
		// Users should not override this function.
		void Start();
	protected:
		using ConnectionPtr = std::shared_ptr<Connection<T>>;
		// This function will be called when there is a new connection request.
		// Users can override this class for further processing. The newly accepted
		// connection will be passed as the argument of this function.
		virtual void OnClientConnect(ConnectionPtr& new_connection);
		virtual void OnClientDisconnect(ConnectionPtr connection);
		virtual void HandleMessage();
	private:
		// Start an asynchronous accept.
		void StartAccept();
		// Callback function that will be called where there is a new connection arrived.
		void HandleAccept(const asio::error_code& error, tcp::socket peer);
	protected:
		std::unordered_map<size_t, ConnectionPtr> m_connections;
		MessageQueue<T> m_message_queue;
		size_t m_connection_count;
		size_t m_id;
	private:
		asio::io_context m_io_context;
		tcp::acceptor m_acceptor;
		std::thread m_thread;
	};

	template<Protocal T>
	TcpServer<T>::TcpServer() : m_io_context(),
		m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), 6000)), 
		m_message_queue(), m_connection_count(0), m_id(1000)
	{
		try
		{
			std::cout << "[SERVER] Started!" << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}

	template<Protocal T>
	void TcpServer<T>::Start()
	{
		StartAccept();
		m_thread = std::thread([this]() { m_io_context.run(); });
		while (true)
		{
			HandleMessage();
		}
	}

	template<Protocal T>
	void TcpServer<T>::OnClientConnect(ConnectionPtr& new_connection)
	{

	}

	template<Protocal T>
	void TcpServer<T>::OnClientDisconnect(ConnectionPtr connection)
	{

	}

	template<Protocal T>
	void TcpServer<T>::HandleMessage()
	{

	}

	template<Protocal T>
	void TcpServer<T>::StartAccept()
	{
		m_acceptor.async_accept(std::bind(&TcpServer::HandleAccept, this, std::placeholders::_1, std::placeholders::_2));
	}

	template<Protocal T>
	void TcpServer<T>::HandleAccept(const asio::error_code& error, tcp::socket peer)
	{
		if (!error)
		{
			ConnectionPtr new_connection = 
				std::make_shared<Connection<T>>(m_connection_count + m_id, m_io_context, std::move(peer), m_message_queue);
			OnClientConnect(new_connection);
		}
		else
		{
			std::cerr << error.message() << std::endl;
		}

		StartAccept();
	}
}

