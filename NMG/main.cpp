#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include "queue.h"
#include "receiver.h"
#include "util.h"

/*
	in main:
	ask the user if they want a server or client

	Later:

	UDP send bcast message
	set udp socket to non blocking
	wait a bit
	try to receive
	if there is a server, the connect to it
	else spawn a server, wait a bit then connect
*/

void server()
{
	// Create a listener to wait for incoming connections
	sf::TcpListener listener;
	// Wait for a connection

	if (listener.listen(TCP_PORT, ((sf::IpAddress)"localhost")) != sf::Socket::Done);
	{
		std::cerr << "TCP Server - error listening\n";
		return;
	}
	std::cerr << "Server connect to Client";
	sf::TcpSocket socket;
	if (listener.accept(socket) != sf::Socket::Done)
	{
		std::cerr << "TCP Server - error accepting\n";
		return;
	}
	// now we can receive messages

	sf::Packet packet;
	if (socket.receive(packet) != sf::Socket::Done)
	{
		std::cerr << "TCP Server - error receiving\n";
		return;
	}
	std::string message;
	packet >> message;
	std::cout << message;
	packet.clear();
	packet << "message got!";

	if (socket.send(packet) != sf::Socket::Done)
	{
		std::cerr << "TCP Server - error sending\n";
		return;
	}
}

void client()
{
	// Create a socket and connect it
	sf::TcpSocket socket;
	sf::IpAddress address("localhost");
	if (socket.connect(address, TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "TCP Client - error connecting\n";
		return;
	}
	std::cerr << "Client connected to server\n";
	// Send a message to the connected host
	sf::Packet packet;
	packet << "Yahhhh das hot";
	if (socket.send(packet) != sf::Socket::Done);
	{
		std::cerr << "TCP Client - error sending\n";
		return;
	}
	packet.clear();
	if (socket.receive(packet) != sf::Socket::Done);
	{
		std::cerr << "TCP Client - error receiving\n";
		return;
	}
	std::string message;
	packet >> message;
	std::cout << message;

}

int main()
{
	std::thread serverThread(&server);
	std::this_thread::sleep_for(std::chrono::microseconds(10));
	client();

	serverThread.join();
	return 0;
}