#include "UDPserver.h"
#include "util.h"

void UDPServer()
{
	sf::UdpSocket UDPsocket;
	UDPsocket.bind(UDPPORT);

	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress senderIp;
	unsigned short senderPort = UDPPORT;

	if (UDPsocket.receive(buffer, sizeof(buffer), received, senderIp, senderPort) != sf::Socket::Done)
	{
		std::cerr << "!!! UDPserver didnt recive message from client\n";
		return;
	}
	std::stringstream ss;
	ss << "UDP: Client connected" + senderIp.toString();
	std::cout << ss.str();

	UDPsocket.send(message.c_str(), message.size() + 1, sender, senderPort);
}
