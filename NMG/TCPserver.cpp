#include "TCPserver.h"
#include "util.h"
#include <list>

class TCPReceiver
{
private:
	std::shared_ptr<sf::TcpSocket> _tSocket;
	std::list<sf::Packet>& _queue;

public:
	TCPReceiver(std::shared_ptr<sf::TcpSocket> TCPsocket, std::list<sf::Packet>& queue) :
		_tSocket(TCPsocket),
		_queue(queue) {};

	void RecLoop()
	{
		std::cout << "TCP RecLoop lauched\n";
		char buffer[256];
		while (1)
		{
			std::memset(buffer, 0, 256); //clear the buffer
			std::size_t received = 0;
			sf::Packet packet;

			std::stringstream ss;
			ss << "TCP RECLOOP: " << _tSocket->getRemoteAddress() << " : " << _tSocket->getRemotePort() << std::endl;
			std::cout << ss.str();

			if (_tSocket->receive(buffer, sizeof(buffer), received) == sf::Socket::Done)
			{
				packet.append(buffer, received);
				_queue.push_back(packet);
			}
		}
	}
};

class Accepter
{
private:
	std::list<sf::Packet>& _queue;
	std::list<std::shared_ptr<sf::TcpSocket>>& _sockets;
public:
	Accepter(std::list<std::shared_ptr<sf::TcpSocket>>& sockets, std::list<sf::Packet>& queue) :
		_sockets(sockets),
		_queue(queue)

	{}

	void AccLoop()
	{
		std::cout << "TCP accLoop lauched\n";
		sf::TcpListener listener;
		if (listener.listen(TCPPORT) != sf::Socket::Done)
		{
			std::cerr << "TCP Error listening to accepter";
			return;
		}
		std::cout << "TCP Bound to port : " << TCPPORT << "\n";

		while (true)
		{
			std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
			if (listener.accept(*socket) != sf::Socket::Done)
			{
				std::cerr << "TCP Cannot accept connection";
				return;
			}
			// Now it is able to accept messages
			_sockets.push_back(socket);
			std::stringstream ss;
			ss << "TCP Accepted a connection from: " << socket->getRemoteAddress() << " : " << socket->getRemotePort() << std::endl;
			std::cout << ss.str();

			TCPReceiver TCPReceiver(socket, _queue);
			std::thread receiverThread(&TCPReceiver::RecLoop, &TCPReceiver);
			receiverThread.detach();
		}
	}
};

void TCPServer()
{
	std::list<std::shared_ptr<sf::TcpSocket>> connectedClients;
	std::list<sf::Packet> queue;

	Accepter accepter(connectedClients, queue);
	std::thread accepterThread(&Accepter::AccLoop, &accepter);
	accepterThread.detach();

	while (1)
	{
		if (!queue.empty())
		{
			ClientInfo recInfo;
			sf::Packet recPack = queue.front(); //this might require a mutex
			recPack >> recInfo;
			queue.pop_front();

			sf::Packet sendPack;
			ClientInfo sendInfo = recInfo; //make a clone of recInfo. I can use these to cross reference if needed when i make changes on the server side

			if (recInfo.type == EMsgType::Setup)
			{
				sendInfo.localID = connectedClients.size() - 1; // hacky type of way to set the localplayer. Relies on the server knowing the correct amount of people connected
				sendInfo.sendToSelf = true;
				sendInfo.sendToOthers = false;
			}
			sendPack << sendInfo;


			// Now send to whoever needs it sent
			for (auto& client : connectedClients)
			{
				if (recInfo.sendToSelf && (client->getRemoteAddress() == recInfo.ip && client->getRemotePort() == recInfo.port))
				{
					client->send(sendPack.getData(), sendPack.getDataSize());
				}
				else if (recInfo.sendToOthers && !(client->getRemoteAddress() == recInfo.ip && client->getRemotePort() == recInfo.port))
				{
					client->send(sendPack.getData(), sendPack.getDataSize());
				}
			}
		}
	}
}
