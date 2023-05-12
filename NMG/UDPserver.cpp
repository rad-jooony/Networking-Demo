#include "UDPserver.h"
#include "util.h"

void UDPServer()
{
	sf::UdpSocket UDPsocket;
	auto status = UDPsocket.bind(UDPPORT);
	if (status != sf::Socket::Done)
	{
		std::cout << "!!! UDP Socket is not bound\n";
	}

	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress senderIp;
	unsigned short senderPort = UDPPORT;

	while (1)
	{
		if (UDPsocket.receive(buffer, sizeof(buffer), received, senderIp, senderPort) != sf::Socket::Done)
		{
			std::cerr << "!!! UDPserver didnt recive message from client\n";
			return;
		}
		std::stringstream ss;
		ss << "UDP: message from " << senderIp.toString() << " -- \n" << (std::string(buffer, received)) << "\n";
		std::cout << ss.str();

		std::string message{ "UDP Test message to the client\n" };
		UDPsocket.send(message.c_str(), message.size() + 1, senderIp, senderPort);
	}
}
