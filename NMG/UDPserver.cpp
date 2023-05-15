#include "UDPserver.h"
#include "util.h"

#include <vector>

void UDPServer() {
	sf::UdpSocket UDPsocket;
	auto status = UDPsocket.bind(UDPPORT);
	if (status != sf::Socket::Done) {
		std::cout << "!!! UDP Socket is not bound\n";
		return;
	}

	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress senderIp;
	unsigned short senderPort = UDPPORT;

	std::vector<ClientInfo> connectedClients;

	while (true) {

		if (UDPsocket.receive(buffer, sizeof(buffer), received, senderIp, senderPort) != sf::Socket::Done)
		{
			std::cerr << "!!! UDP server socket disconnected --- server closed\n";
			return;
		}

		std::stringstream ss;
		ss << "UDP: message from " << senderIp.toString() << " of size " << received << " bytes \n";
		std::cout << ss.str();


		// check the senders IP and port in the client list. Use this to figure out if its a new client and add them to the client list (I should move this to the TCP server)
		bool senderFound = false;
		for (const auto& client : connectedClients)
		{
			if (client.ip == senderIp && client.port == senderPort)
			{
				senderFound = true;
				break;
			}
		}
		if (!senderFound)
			connectedClients.push_back({ senderIp, senderPort });

		ClientInfo recInfo;
		// HERE, MAKE A REC LOOP INSTEAD
		sf::Packet recPacket;
		recPacket.append(buffer, received);
		// If i figure out how to enum classes or something, do a check here for different message types



		 // Send the message to all connected clients except the sender
		for (const auto& client : connectedClients)
		{
			if (client.ip != senderIp || client.port != senderPort) 
			{
				UDPsocket.send(recPacket.getData(), recPacket.getDataSize(), client.ip, client.port);
			}
		}
	}
}
