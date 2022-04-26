#pragma once

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <vector>

namespace net
{
	template<typename T>
	class MessageQueue
	{
	public:
		T ReadMessage()
		{
			T message = m_message_in.front();
			m_message_in.erase(m_message_in.begin());
			return message;
		}

		void WriteMessage(const T& message)
		{
			m_message_out.push_back(message);
		}

		bool Empty()
		{
			return m_message_in.empty();
		}
	private:
		std::vector<T> m_message_in;
		std::vector<T> m_message_out;
	};
}

#endif



