#include "UDPserver.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <list>

class UDPReceiver
{
private:
	sf::UdpSocket* _uSocket;
	std::list<sf::Packet>& _queue;
public:
	UDPReceiver(sf::UdpSocket* UDPsocket, std::list<sf::Packet>& queue) :
		_uSocket(UDPsocket),
		_queue(queue) {};

	void RecLoop()
	{
		char buffer[256];
		while (1)
		{
			std::memset(buffer, 0, 256); //clear the buffer
			std::size_t received = 0;
			sf::Packet packet;
			sf::IpAddress senderIp;
			unsigned short senderPort = UDPPORT;

			if (_uSocket->receive(buffer, sizeof(buffer), received, senderIp, senderPort) == sf::Socket::Done)
			{
				packet.append(buffer, received);
				_queue.push_back(packet);
			}
		}
	}
};

void UDPServer() 
{
	sf::UdpSocket UDPsocket;
	auto status = UDPsocket.bind(UDPPORT);
	if (status != sf::Socket::Done) {
		std::cout << "!!! UDP Socket is not bound\n";
		return;
	}

	std::map<sf::IpAddress, unsigned short> connectedClients;
	std::list<sf::Packet> queue;
	UDPReceiver UDPReceiver(&UDPsocket, queue);
	std::thread receiverThread(&UDPReceiver::RecLoop, UDPReceiver);
	receiverThread.detach();

	while (true) {

		if (queue.size() != 0)
		{
			ClientInfo recInfo;
			sf::Packet recPack = queue.front(); //this might require a mutex
			recPack >> recInfo;
			queue.pop_front();
			// check the senders IP and port in the client list. Use this to figure out if its a new client and add them to the client list (I should move this to the TCP server)
			if (recInfo.type != EMsgType::positionUpdate)
			{
				// bad packet
				continue;
			}
			
			bool senderFound = false;
			for (const auto& client : connectedClients)
			{
				if (client.first == recInfo.ip && client.second == recInfo.port)
				{
					senderFound = true;
					break;
				}
			}
			if (!senderFound)
				connectedClients.insert({ recInfo.ip, recInfo.port });

			 // Send the message to all connected clients except the sender
			for (const auto& client : connectedClients)
			{
				if (client.first != recInfo.ip || client.second != recInfo.port) //dont send to self
				{
					UDPsocket.send(recPack.getData(), recPack.getDataSize(), client.first, client.second);
				}
			}
		}

		// do heartbeat check here

	}
}
