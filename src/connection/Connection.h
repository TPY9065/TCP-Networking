#pragma once
#include <string>
#include <string_view>
#include "core.hpp"
#include "message_queue.h"

using asio::ip::tcp;

namespace net
{
	template<Protocal T>
	class Connection : public std::enable_shared_from_this<Connection<T>>
	{
	public:
		Connection(size_t id, asio::io_context& io_context, tcp::socket socket, MessageQueue<T>& messageQueue);
		void ConnectToServer(const tcp::resolver::results_type& endpoints);
		void ConnectToClient();
		void Disconnect();
		void ReadMessage();
		void WriteMessage(const Message<T>& message);
		bool IsOpen() const;
		size_t GetId() const;
	private:
		void ReadMessageHeader();
		void ReadMessageBody();
		void WriteMessageHeader();
		void WriteMessageBody();
		void ConnectionHandler(const asio::error_code& error, const tcp::endpoint& endpoint);
		// If header is received successfully, resize the buffer size of the body of message in, 
		// and start waiting the message body. Otherwise, discard current message and wait for next message header.
		void ReadHeaderHandler(const asio::error_code& error, size_t bytes_transferred);
		// If body is received successfully, add the received message to the message queue. Then wait 
		// for the next message header. Otherwise, discard current message and wait for next message header.
		void ReadBodyHandler(const asio::error_code& error, size_t bytes_transferred);
		// If header is sent successfully, send the message body.
		// Otherwise, resent current message header. If error occurred, diconnect current connection.
		void WriteHeaderHandler(const asio::error_code& error, size_t bytes_transferred);
		// If body is sent successfully, send next message header.
		// Otherwise, resent current message header until client received. If error occurred,
		// disconnect current connection.
		void WriteBodyHandler(const asio::error_code& error, size_t bytes_transferred);
		void LogError(const asio::error_code& error, const std::string_view& functor);
	protected:
		size_t m_id;
	private:
		asio::io_context& m_io_context;
		tcp::socket m_socket;
		Message<T> m_message_in;
		Message<T> m_message_out;
		MessageQueue<T>& m_message_queue;
	};

	template<Protocal T>
	Connection<T>::Connection(size_t id, asio::io_context& io_context, tcp::socket socket, MessageQueue<T>& messageQueue) :
		m_id(id), m_io_context(io_context), m_socket(std::move(socket)), m_message_in(), m_message_out(), m_message_queue(messageQueue)
	{

	}

	template<Protocal T>
	void Connection<T>::ConnectToServer(const tcp::resolver::results_type& endpoints)
	{
		asio::async_connect(m_socket, endpoints,
			std::bind(&Connection::ConnectionHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	template<Protocal T>
	void Connection<T>::ConnectToClient()
	{

	}

	template<Protocal T>
	inline void Connection<T>::Disconnect()
	{
		asio::error_code error;
		m_socket.shutdown(tcp::socket::shutdown_both, error);
		m_socket.close(error);
		if (error)
			std::cerr << "Disconnect Error: " << error.message() << std::endl;
	}

	template<Protocal T>
	void Connection<T>::ReadMessage()
	{
		ReadMessageHeader();
	}

	template<Protocal T>
	void Connection<T>::WriteMessage(const Message<T>& message)
	{
		m_message_queue.WriteMessageOut(message);
		WriteMessageHeader();
	}

	template<Protocal T>
	bool Connection<T>::IsOpen() const
	{
		return m_socket.is_open();
	}

	template<Protocal T>
	size_t Connection<T>::GetId() const
	{
		return m_id;
	}

	template<Protocal T>
	void Connection<T>::ReadMessageHeader()
	{
		asio::async_read(m_socket, asio::buffer(&m_message_in.header, sizeof(Header<T>)),
			std::bind(&Connection::ReadHeaderHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	template<Protocal T>
	void Connection<T>::ReadMessageBody()
	{
		asio::async_read(m_socket, asio::buffer(m_message_in.body.data(), m_message_in.body.size() * sizeof(Message<T>::byte)),
			std::bind(&Connection::ReadBodyHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	template<Protocal T>
	void Connection<T>::WriteMessageHeader()
	{
		if (!m_message_queue.MessageOutEmpty())
		{
			m_message_out = m_message_queue.ReadMessageOut();
			asio::async_write(m_socket, asio::buffer(&m_message_out.header, sizeof(Header<T>)),
				std::bind(&Connection::WriteHeaderHandler, this, std::placeholders::_1, std::placeholders::_2));
		}
	}

	template<Protocal T>
	void Connection<T>::WriteMessageBody()
	{
		asio::async_write(m_socket, asio::buffer(m_message_out.body.data(), m_message_out.body.size() * sizeof(Message<T>::byte)),
			std::bind(&Connection::WriteBodyHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	template<Protocal T>
	void Connection<T>::ConnectionHandler(const asio::error_code& error, const tcp::endpoint& endpoint)
	{
		if (!error)
		{
			ReadMessage();
		}
		else
		{
			std::cerr << "ConnectionHandler: " << error.message() << std::endl;
		}
	}
	
	template<Protocal T>
	void Connection<T>::ReadHeaderHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			if (bytes_transferred == sizeof(Header<T>))
			{
				m_message_in.body.resize(m_message_in.header.size);
				ReadMessageBody();
			}
			else
			{
				ReadMessageHeader();
			}
		}
		else
		{
			LogError(error, "ReadHeaderHandler");
			Disconnect();
		}
	}

	template<Protocal T>
	void Connection<T>::ReadBodyHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			if (m_message_in.size_in_bytes() == bytes_transferred)
			{
				m_message_queue.WriteMessageIn(m_message_in);
			}
			ReadMessageHeader();
		}
		else
		{
			LogError(error, "ReadBodyHandler");
			Disconnect();
		}
	}

	template<Protocal T>
	void Connection<T>::WriteHeaderHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			if (bytes_transferred == sizeof(Header<T>))
			{
				WriteMessageBody();
			}
			else
			{
				WriteMessageHeader();
			}
		}
		else if (error)
		{
			LogError(error, "WriteHeaderHandler");
			Disconnect();
		}
	}

	template<Protocal T>
	void Connection<T>::WriteBodyHandler(const asio::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			if (m_message_out.size_in_bytes() == bytes_transferred)
			{
				m_message_queue.PopMessageOut();
			}
			WriteMessageHeader();
		}
		else if (error)
		{
			LogError(error, "WriteBodyHandler");
			Disconnect();
		}
	}
	template<Protocal T>
	void Connection<T>::LogError(const asio::error_code& error, const std::string_view& functor)
	{
		std::cerr << "ID[" << m_id << "] " << functor << " Error: " << error.message() << std::endl;
	}
}

