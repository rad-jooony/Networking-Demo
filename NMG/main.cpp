#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#define UDPPORT 54000
#define CLIENTPORT 54001

void Server()
{
	sf::UdpSocket socket;

	if (socket.bind(UDPPORT) != sf::Socket::Done) //the server must bind in order to receive messages 
	{
		std::cerr << "UDP: Socket not binding - Port : " << UDPPORT << "\n";
		return;
	}
	std::cout << "Port Connected : " << UDPPORT << "\n"; // now the server can receive messages

	std::string data = "This Is The Server"; //to send to client

	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress sender;
	unsigned short port;

	while (true) //now forever recive and send >:))))))))
	{
		socket.receive(buffer, sizeof(buffer), received, sender, port); // take in all the info

		std::string message(buffer, received); // make the message not overflow with shit
		std::cout << "UDP: Message from client : " << message << "\n";

		socket.send(data.c_str(), data.size(), sender, CLIENTPORT); // on message received, send stuff
	}
}

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
	while (1)
	{
		std::cout << "select what to run - Client/Server - (c/s)\n";
		std::string pick;
		std::cin >> pick;

		if (pick == "s")
		{
			std::thread TCPserverThread(&Server);
			TCPserverThread.detach();
		}

		if (pick == "c")
		{
			Client();
		}
	}
	return 0;
}
