#include "UDPserver.h"
#include <iostream>
#include "Util.h"


void UDPserver()
{
	sf::UdpSocket socket;

	if (socket.bind(UDP_PORT) != sf::Socket::Done) //the server must bind in order to receive messages 
	{
		std::cerr << "UDP: Socket not binding - Port : " << UDP_PORT << "\n";
		return;
	}
	std::stringstream ss;
	ss << "UDP Port Connected : " << UDP_PORT << "\n"; // now the server can receive messages
	std::cout << ss.str();

	std::string data = "Hello Client, this is the server"; //to send to client

	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress sender;
	unsigned short port;

	while (true) 
	{
		socket.receive(buffer, sizeof(buffer), received, sender, port); // take in all the info

		std::string message(buffer, received); // make the message not overflow with shit
		std::cout << "UDP: Message from client : " << message << "\n";

		socket.send(data.c_str(), data.size(), sender, PORT); // on message received, send stuff
	}
}