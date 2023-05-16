#include <iostream>
#include <thread>
#include "util.h"
#include <SFML/Network.hpp>
#include "UDPServer.h"
#include "TCPServer.h"
#include <list>

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
	std::shared_ptr<sf::TcpSocket> _tSocket;
	sf::UdpSocket* _uSocket;
	std::list<sf::Packet>& _queue;
public:
	ClientReceiver(sf::UdpSocket* UDPsocket, std::shared_ptr<sf::TcpSocket> TCPsocket, std::list<sf::Packet>& queue) :
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
		_uSocket->setBlocking(false); // remove blocking from TCPsocket


		// If I have time i should handle this better. Right now this feels very 'hacky'
		while (1)
		{
			std::memset(buffer, 0, 256); //clear the buffer
			std::size_t received;
			sf::Packet packet;
			sf::IpAddress UDPIp = "127.0.0.1"; //this address specifically refers to the local machine
			unsigned short UDPPort = UDPPORT;
			// first check from the TCP client

			auto tstatus = _tSocket->receive(buffer, 256, received);
			if (tstatus == sf::Socket::Done)
			{
				// append the buffer to the packet
				packet.append(buffer, received);

				std::stringstream ss;
				std::cout << "From TCP - Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
				std::cout << ss.str();

				_queue.push_back(packet); // place info at the back
			}
			else if (tstatus == sf::Socket::Disconnected)
			{
				// handle socket disconnection
				std::cout << "RecvLoop Dropped";
				break;
			}

			auto ustatus = _uSocket->receive(buffer, sizeof(buffer), received, UDPIp, UDPPort);
			if (ustatus == sf::Socket::Done)
			{
				packet.append(buffer, received);
				//std::stringstream ss;
				//std::cout << "From UDP - Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
				//std::cout << ss.str();
				_queue.push_back(packet);
			}

			if (ustatus == sf::Socket::NotReady || tstatus == sf::Socket::NotReady)
			{
				//sf::sleep(sf::milliseconds(10)); //sleep to ease up on the CPU
			}
		}
	}
};

void Game(sf::RenderWindow& window); //prototype

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
			Game(window);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !keypress) // Select Client
		{
			keypress = true;
			Game(window);
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

void Game(sf::RenderWindow& window)
{
	// TCP SERVER CONNECT
	std::shared_ptr<sf::TcpSocket> TCPsocket = std::make_shared<sf::TcpSocket>();
	//unsigned int ClientPort = CLIENTPORT; // it would make more sense to dynamically generate a port, though maybe not necessary

	if (TCPsocket->connect(sf::IpAddress::getLocalAddress(), TCPPORT) != sf::Socket::Done)
	{
		std::stringstream ss;
		ss << "!!! Client Failed to connect to TCP socket -- Port is: " << TCPPORT << std::endl;
		std::cout << ss.str();
		return;
	}

	// UDP SERVER CONNECT
	sf::UdpSocket UDPsocket;
	unsigned int ClientPort;
	if (UDPsocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) // Client binds UDPsocket to their own port
	{
		ClientPort = UDPsocket.getLocalPort();
		std::stringstream ss;
		ss << "!!! Client Failed to bind UDP port -- Port is : " << ClientPort << std::endl;
		std::cout << ss.str();
		return;
	}

	ClientPort = UDPsocket.getLocalPort();
	std::stringstream ss;
	ss << "Client bound UDP server to port : " << ClientPort << std::endl;
	std::cout << ss.str();
	ss.clear();
	unsigned short serverPort = UDPPORT;
	std::size_t received = 0;
	sf::IpAddress serverIp = "127.0.0.1"; //This IP refers to the local machine

	//LAUNCH CLIENT REC
	std::list<sf::Packet> queue;
	ClientReceiver clientReceiver(&UDPsocket, TCPsocket, queue);
	std::thread receiverThread(&ClientReceiver::RecLoop, clientReceiver);
	receiverThread.detach();

	//SEND AND REC MESSAGE FOR PLAYER SETUP
	ClientInfo setupInfo;
	setupInfo.type = EMsgType::Setup;
	sf::Packet setupPack;
	setupPack << setupInfo;
	if (TCPsocket->send(setupPack.getData(), setupPack.getDataSize()) != sf::Socket::Done)
	{
		ss << "!!! Client Failed to send setup info to TCPserver\n";
		std::cout << ss.str();
		return; //I should handle this error differently, such as retrying 
	}

	if (queue.empty())
	{
		std::cerr << "Client queue is empty. Server has not yet send back setup pack\n";
		while (queue.empty())
		{
		}
	}

	std::cerr << "pack recevied\n";
	sf::Packet recPack = queue.front(); //this might require a mutex
	ClientInfo recInfo;
	recPack >> setupInfo;
	if (setupInfo.type == EMsgType::Setup)
	{
		queue.pop_front();

	}
	else
	{
		std::cerr << "!!! Message is not setup pack, returing\n";
		return;
	}


	//GAME INITIALISATION
	window.setFramerateLimit(60);
	sf::CircleShape playerIcon;
	playerIcon.setRadius(5);
	int localPlayer{ setupInfo.localID };
	const int players{ 3 };
	std::vector<sf::Color> colours{ sf::Color::Blue,sf::Color::Green, sf::Color::Red };
	std::vector<Player> player(players, Player{});

	ClientInfo myInfo;
	myInfo.ip = "localhost";
	myInfo.port = ClientPort;
	myInfo.localID = localPlayer;
	myInfo.type = EMsgType::positionUpdate;

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

			if (recInfo.localID != localPlayer)
				player[recInfo.localID].pos = recInfo.pos;
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
