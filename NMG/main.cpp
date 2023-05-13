#include <iostream>
#include <thread>
#include "util.h"
#include <SFML/Network.hpp>
#include "UDPServer.h"
#include "TCPServer.h"


void UDPClient()
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

struct Player {
	sf::Vector2f pos;
	sf::Color colour;

	void move()
	{
		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
		{
			--pos.y;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			++pos.y;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			--pos.x;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			++pos.x;
		}
	}
};

void game(sf::RenderWindow& window); //prototype

void join();

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Network Testing");
	sf::Font font;
	if (!font.loadFromFile("data/unispace bd.ttf"))
	{
		std::cerr << "ERROR: cannot find font\n";
		return false;
	}
	sf::Text text;
	text.setFont(font);

	bool keypress{ false };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::H) && !keypress) // Select Host
		{
			keypress = true;
			std::thread TCPserverThread(&TCPServer);
			TCPserverThread.detach();
			std::thread UDPserverThread(&UDPServer);
			UDPserverThread.detach();
			game(window);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !keypress) // Select Client
		{
			join();
		}

		if (keypress == true && !sf::Keyboard::isKeyPressed(sf::Keyboard::H) && !sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			keypress = false;
		}

		window.clear();
		window.display();
	}
	return 0;
}

void game(sf::RenderWindow& window)
{
	// TCP SERVER CONNECT






	// UDP SERVER CONNECT
	sf::UdpSocket UDPsocket;
	if (UDPsocket.bind(CLIENTPORT) != sf::Socket::Done) // Client binds UDPsocket to their own port
	{
		std::stringstream ss;
		ss << "!!! Client Failed to bind UDP port -- Port is : " << CLIENTPORT << std::endl;
		std::cout << ss.str();
		return;
	}
	std::stringstream ss;
	ss << "Client bound UDP server to port : " << CLIENTPORT << std::endl;
	std::cout << ss.str();
	ss.clear();

	// UDP test message
	unsigned short serverPort = UDPPORT;
	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress serverIp = "127.0.0.1"; //This IP refers to the local machine
	std::string message = "Client to UDPServer :: test message : IP  -- " + sf::IpAddress::getLocalAddress().toString() + "\n";
	UDPsocket.send(message.c_str(), message.size() + 1, serverIp, 55002);

	if (UDPsocket.send(message.c_str(), message.size(), serverIp, UDPPORT) != sf::Socket::Done) 
	{
		std::cerr << "!!! Client could not send message to UDP server\n";
		return;
	}

	sf::Socket::Status status;
	sf::sleep(sf::seconds(1));
	status = UDPsocket.receive(buffer, sizeof(buffer), received, serverIp, serverPort);

	

	if (status == sf::Socket::Done)
	{
		std::string recMessage(buffer, received);
		ss.clear();
		ss << "Client: Message from UDPserver : " << recMessage << "\n";
		std::cout << ss.str();
	}
	else
	{
		std::cout << "socket status not done\n";
	}




	//GAME INITIALISATION
	window.setFramerateLimit(60);
	sf::CircleShape playerIcon;
	playerIcon.setRadius(5);
	int localPlayer{ 0 };
	const int players{ 2 };
	std::vector<sf::Color> colours{ sf::Color::Blue,sf::Color::Green, sf::Color::Red };
	std::vector<Player> player(players, Player{});


	// GAME RUNNING
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		//LOCAL UPDATES
		player[localPlayer].move();

		//SERVER UPDATES


		// RENDERING
		window.clear();
		for (int i = 0; i < players; i++)
		{
			playerIcon.setFillColor(colours[i]);
			playerIcon.setPosition(player[i].pos);
			window.draw(playerIcon);
		}
		window.display();
	}
}

void join()
{
}
