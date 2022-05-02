#include "client/tcp_client.h"

enum class Protocal
{
	SERVER, CLIENT
};

class Client : public net::TcpClient<Protocal>
{
public:
	void HandleMessage()
	{
		while (true)
		{
			if (!m_messages_queue.MessageInEmpty())
			{
				net::Message<Protocal> message = m_messages_queue.ReadMessageIn();
				std::cout << "Message sent from server: \n" << message.to_json() << std::endl;
				m_messages_queue.PopMessageIn();
			}
		}
	}
};

int main()
{
	Client client;
	if (client.Connect("127.0.0.1", "6000"))
	{
		client.HandleMessage();
	}
	return 0;
}