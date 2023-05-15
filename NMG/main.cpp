#include <iostream>
#include <thread>
#include "util.h"
#include <SFML/Network.hpp>
#include "UDPServer.h"
#include "TCPServer.h"
#include <list>


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

struct Player
{
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

class ClientReceiver
{
private:
	sf::TcpSocket* _tSocket;
	sf::UdpSocket* _uSocket;
	std::list<sf::Packet>& _queue;
public:
	ClientReceiver(sf::UdpSocket* UDPsocket, sf::TcpSocket* TCPsocket, std::list<sf::Packet>& queue) :
		_tSocket(TCPsocket),
		_uSocket(UDPsocket),
		_queue(queue) {};

	void RecLoop()
	{
		char buffer[256];
		{
			std::stringstream ss;
			ss << "Receiver :: " << _tSocket->getRemoteAddress() << ":" << _tSocket->getRemotePort() << std::endl;
			std::cout << ss.str();
		}
		_tSocket->setBlocking(false); // remove blocking from TCPsocket
		while (1)
		{
			std::memset(buffer, 0, 256); //clear the buffer
			std::size_t received;
			sf::Packet packet;
			sf::IpAddress UDPIp = "127.0.0.1"; //this address specifically refers to the local machine
			unsigned short UDPPort = UDPPORT;
			// first check from the TCP client

			auto status = _tSocket->receive(buffer, 256, received);
			if (status == sf::Socket::Done)
			{
				// append the buffer to the packet
				packet.append(buffer, received);
				{
					std::stringstream ss;
					std::cout << "From TCP - Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
					std::cout << ss.str();
				}
				_queue.push_back(packet); // place info at the back
			}
			else if (status == sf::Socket::Disconnected)
			{
				// handle socket disconnection
				std::cout << "RecvLoop Dropped";
				break;
			}

			status = _uSocket->receive(buffer, sizeof(buffer), received, UDPIp, UDPPort);
			if (status == sf::Socket::Done)
			{
				packet.append(buffer, received);
				std::stringstream ss;
				std::cout << "From UDP - Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
				std::cout << ss.str();
				_queue.push_back(packet);
			}
		}
	}
};

void Game(sf::RenderWindow& window, unsigned int, int); //prototype

void Join();

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
			Game(window, CLIENTPORT, 0);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !keypress) // Select Client
		{
			keypress = true;
			Game(window, 4001, 1);
			Join();
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

void Game(sf::RenderWindow& window, unsigned int ClientPort, int TlocalPlayer)
{
	// TCP SERVER CONNECT
	sf::TcpSocket TCPsocket;


	// UDP SERVER CONNECT
	sf::UdpSocket UDPsocket;
	if (UDPsocket.bind(ClientPort) != sf::Socket::Done) // Client binds UDPsocket to their own port
	{
		std::stringstream ss;
		ss << "!!! Client Failed to bind UDP port -- Port is : " << ClientPort << std::endl;
		std::cout << ss.str();
		return;
	}
	std::stringstream ss;
	ss << "Client bound UDP server to port : " << ClientPort << std::endl;
	std::cout << ss.str();
	ss.clear();

	// UDP test message
	unsigned short serverPort = UDPPORT;
	//char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress serverIp = "127.0.0.1"; //This IP refers to the local machine
	std::string message = "Client to UDPServer :: test message : IP  -- " + sf::IpAddress::getLocalAddress().toString() + "\n";

	if (UDPsocket.send(message.c_str(), message.size(), serverIp, UDPPORT) != sf::Socket::Done)
	{
		std::cerr << "!!! Client could not send message to UDP server\n";
		return;
	}

	//Launch client Receiver 
	std::list<sf::Packet> queue;
	ClientReceiver clientReceiver(&UDPsocket, &TCPsocket, queue);
	std::thread receiverThread(&ClientReceiver::RecLoop, clientReceiver);
	receiverThread.detach();

	//GAME INITIALISATION
	window.setFramerateLimit(60);
	sf::CircleShape playerIcon;
	playerIcon.setRadius(5);
	int localPlayer{ TlocalPlayer };
	const int players{ 2 };
	std::vector<sf::Color> colours{ sf::Color::Blue,sf::Color::Green, sf::Color::Red };
	std::vector<Player> player(players, Player{});

	ClientInfo myInfo;
	myInfo.ip = "localhost";
	myInfo.port = CLIENTPORT;


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
		auto temppos = player[localPlayer].pos;
		player[localPlayer].move();

		//SEND TO SERVER
		if (myInfo.pos != temppos)
		{
			myInfo.pos = player[localPlayer].pos;
			sf::Packet pack;
			pack << myInfo;
			UDPsocket.send(pack.getData(), pack.getDataSize(), serverIp, serverPort);
		}

		//SERVER UPDATES
		while (!queue.empty())
		{
			ClientInfo recInfo;
			sf::Packet recPack = queue.front(); //this might require a mutex
			recPack >> recInfo;
			queue.pop_front();
			if (TlocalPlayer == 0)
				player[1].pos = recInfo.pos;
			else 
				player[0].pos = recInfo.pos;
		}


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

void Join()
{
}
