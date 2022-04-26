#include "client/TCP_Client.h"

int main()
{
	net::TCP_Client client;
	client.Connect("127.0.0.1", "6000");
	while (true)
	{

	}
	return 0;
}