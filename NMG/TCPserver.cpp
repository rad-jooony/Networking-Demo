#include "TCPserver.h"
#include "receiver.h"
#include "util.h"
#include <iostream>
#include <thread>
#include "ClientInfo.h"

Accepter::Accepter(Queue<sf::Packet>& q, List<std::shared_ptr<sf::TcpSocket>>& s) :
	_queue(q),
	_socket(s)
{}

void Accepter::operator()()//double bracket makes this a function
{
	sf::TcpListener listener;
	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "TCP Error listening to accepter";
		return;
	}
	std::cout << "TCP Bound to port : " << TCP_PORT << "\n";

	while (true)
	{
		std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
		if (listener.accept(*socket) != sf::Socket::Done)
		{
			std::cerr << "TCP Cannot accept connection";
			return;
		}
		// Now it is able to accept messages

		_socket.push(socket); //add this socket to the list of sockets

		std::stringstream ss;
		ss << "TCP Accepted a connection from: " << socket->getRemoteAddress() << ":" << socket->getRemotePort() << std::endl;
		std::cout << ss.str();

		std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, _queue);
		// launch a thread to receive with the receiver
		std::thread(&Receiver::recvLoop, receiver).detach(); // thread lauches detached, meaning it doesnt need to wait to catch up

		/// SEND  CLIENT INFO TO THIS PORT FOR SETUP
		ClientInfo newClient;
		newClient.id = _socket.size() - 1; //The number of sockets connected
		sf::Packet pack;
		pack << newClient;
		std::cout << "TCP clients" << _socket.size() << "\n";
		socket->send(pack.getData(), pack.getDataSize());
	}
}


void TCPserver() 
{
	std::cout << "launching TCPServer\n";
	Queue<sf::Packet> queue;
	List<std::shared_ptr<sf::TcpSocket>> sockets;

	std::thread accThread(Accepter(queue, sockets)); // an accepter thread is launched. This creates a thread that will accept incoming connections.

	while (1)
	{
		sf::Packet pack;
		pack << queue.pop();
		std::stringstream ss;
		std::cout << "TCP Server read: \"" << pack << "\"\n";
		std::cout << ss.str();
		auto send_to_one = [&pack](std::shared_ptr<sf::TcpSocket> socket)
		{
			socket->send(pack.getData(), pack.getDataSize());
		};

		if (pack.getDataSize() > 0) // if there is data
			sockets.for_each(send_to_one);
	}
	return;
}
