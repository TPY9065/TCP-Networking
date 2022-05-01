#pragma once

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <queue>
#include "core.hpp"

namespace net
{
	template<Protocal T>
	struct Header
	{
		size_t size;
		size_t from;
		size_t dest;
		T protocal;
	};

	template<Protocal T>
	struct Message
	{
		using byte = size_t;

		Header<T> header;
		std::vector<byte> body;

		Message<T>& operator=(const std::vector<byte>& data)
		{
			body = data;
			header.size = data.size() * sizeof(byte);
			return *this;
		}
	};

	// A thread-safe queue that has two queues for storing messages sent from and to server.
	// It use a scoped lock to lock current thread to prevents race condition.
	template<Protocal T>
	class MessageQueue
	{
	public:
		Message<T> ReadMessageIn();
		void WriteMessageIn(const Message<T>& message);
		Message<T> ReadMessageOut();
		void WriteMessageOut(const Message<T>& message);
		void PopMessageIn();
		void PopMessageOut();
		bool MessageInEmpty();
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

#endif



