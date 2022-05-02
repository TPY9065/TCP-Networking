#pragma once
#include <iostream>
#include <queue>
#include "core.hpp"

namespace net
{
	template<Protocal T>
	struct Header
	{
		size_t size = 0;
		size_t from = 0;
		size_t dest = 0;
		T protocal;
	};

	template<Protocal T>
	struct Message
	{
		using byte = size_t;

		Header<T> header;
		std::vector<byte> body;

		// Factory method for constructing message in a easier way
		static Message<T> ConstructMessage(T protocal, size_t from, size_t dest, const std::vector<byte>& data)
		{
			Message<T> message;
			message.header.protocal = protocal;
			message.header.from = from;
			message.header.dest = dest;
			message.body = data;
			message.header.size = data.size();
			return message;
		}

		static Message<T> ConstructMessage(T protocal, const std::vector<byte>& data)
		{
			Message<T> message;
			message.header.protocal = protocal;
			message.body = data;
			message.header.size = data.size();
			return message;
		}

		// Returns the size of data in bytes
		size_t size_in_bytes()
		{
			return body.size() * sizeof(byte);
		};

		// Functions for user to write data in a easier way with operator<<, size of header will be re-calculated.
		Message<T>& operator<<(std::vector<byte> datas)
		{
			body = datas;
			header.size = body.size();
			return *this;
		}

		// Same as above, push append one data into body everytime.
		Message<T>& operator<<(byte data)
		{
			body.push_back(data);
			header.size = body.size();
			return *this;
		}

		// Output the message in json format
		std::string to_json()
		{
			std::string json =
				"{\n  header: \n    size: " + std::to_string(header.size) +
				 "\n    from: " + std::to_string(header.from) +
				 "\n    dest: " + std::to_string(header.dest) + 
				 "\n  body: \n    data: ";
			for (byte data : body)
				json += std::to_string(data);
			json = json + "\n" + "}";

			return json;
		}
	};

	// A thread-safe queue that has two queues for storing messages sent from and to server.
	// It use a scoped lock to lock current thread to prevents race condition.
	template<Protocal T>
	class MessageQueue
	{
	public:
		// Gets the first received message in the queue.
		Message<T> ReadMessageIn();
		// Adds a new received message into the queue.
		void WriteMessageIn(const Message<T>& message);
		// Gets the next message that will be sent in the queue.
		Message<T> ReadMessageOut();
		// Adds a new message that will be sent into the queue.
		void WriteMessageOut(const Message<T>& message);
		// Removes the first received message in the queue.
		void PopMessageIn();
		// Removes the first sent message in the queue.
		void PopMessageOut();
		// Returns true if the received message queue is empty, otherwise false.
		bool MessageInEmpty();
		// Returns true if the sent message queue is empty, otherwise false.
		bool MessageOutEmpty();
	private:
		std::queue<Message<T>> m_messages_in;
		std::queue<Message<T>> m_messages_out;
		std::mutex m_mutex;
	};

	template<Protocal T>
	Message<T> MessageQueue<T>::ReadMessageIn()
	{
		std::scoped_lock lock(m_mutex);
		return m_messages_in.front();
	}

	template<Protocal T>
	void MessageQueue<T>::WriteMessageIn(const Message<T>& message)
	{
		std::scoped_lock lock(m_mutex);
		m_messages_in.push(message);
	}

	template<Protocal T>
	Message<T> MessageQueue<T>::ReadMessageOut()
	{
		std::scoped_lock lock(m_mutex);
		return m_messages_out.front();
	}

	template<Protocal T>
	void MessageQueue<T>::WriteMessageOut(const Message<T>& message)
	{
		std::scoped_lock lock(m_mutex);
		m_messages_out.push(message);
	}

	template<Protocal T>
	void MessageQueue<T>::PopMessageIn()
	{
		std::scoped_lock lock(m_mutex);
		if (m_messages_in.empty())
			return;
		m_messages_in.pop();
	}	
	
	template<Protocal T>
	void MessageQueue<T>::PopMessageOut()
	{
		std::scoped_lock lock(m_mutex);
		if (m_messages_out.empty())
			return;
		m_messages_out.pop();
	}

	template<Protocal T>
	bool MessageQueue<T>::MessageInEmpty()
	{
		std::scoped_lock lock(m_mutex);
		return m_messages_in.empty();
	}

	template<Protocal T>
	bool MessageQueue<T>::MessageOutEmpty()
	{
		std::scoped_lock lock(m_mutex);
		return m_messages_out.empty();
	}
}



