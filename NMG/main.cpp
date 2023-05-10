#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include "UDPServer.h"
#include "TCPServer.h"


void Client()
{
	sf::UdpSocket socket; // client needs a socket to connect to?
	if (socket.bind(CLIENTPORT) != sf::Socket::Done) //client needs socket to receive from the server
	{
		std::cerr << "Client: Socket not binding - Port : " << CLIENTPORT << "\n";
		return;
	}
	std::cout << "Client connected to port : " << CLIENTPORT << "\n";

	std::string message = "Hello from client";
	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress serverIp = "127.0.0.1"; //this address specifically refers to the local machine
	unsigned short serverPort = UDPPORT; //for whatever reason the socket.send() doesnt like ints (or at least raw numbers) but will take unsigned shorts 

	if (socket.send(message.c_str(), message.size(), serverIp, serverPort) != sf::Socket::Done)
	{
		std::cerr << "Client: Failed to send message to server\n";
		return;
	}

	if (socket.receive(buffer, sizeof(buffer), received, serverIp, serverPort) != sf::Socket::Done)
	{
		std::cerr << "Client: Failed to receive message from server\n";
		return;
	}
	std::string recMessage(buffer, received); // get the massage and clean it up (its a mess with just buffer)
	std::cout << "Client: Message from Server : " << recMessage << "\n";
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}
