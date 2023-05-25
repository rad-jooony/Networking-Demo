#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include <list>

#include "Game.h"
#include "util.h"
#include "UDPServer.h"
#include "TCPServer.h"

/*
b1634101 Jonathan Braine 
Network and Multiplayer Games Project

Main.cpp includes Client Receiver, main menu and game logic. 
Server and client are both launched from this project

TCP and UDP server function is entirely contained within their respective files

Arrow keys to go move, space/enter to select in menues
esc to quit game/menu

*/

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
				// sf::sleep(sf::milliseconds(10)); //sleep to ease up on the CPU
			}
		}
	}
};

void Run(sf::RenderWindow& window);

// MAIN MENU --- INCLUDES SERVER ESTABLISMENT AND JOIN FUNCTION
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

	// establish options that can be picked
	std::vector<std::string> choices;
	choices.push_back("Host Game");
	choices.push_back("Join Game");
	choices.push_back("Quit");
	int selectedOption{ 0 };
	bool keypress{ false };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		if (window.hasFocus())
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !keypress) // Select up
			{
				keypress = true;
				if (selectedOption > 0)
					selectedOption--;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !keypress) // Select down
			{
				keypress = true;
				if (selectedOption < choices.size() - 1)
					selectedOption++;
			}
			if (keypress && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) // disable option switching until no keys are down
				keypress = false;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) 
			{
				if (selectedOption == 0) // Select Host
				{
					keypress = true;
					std::thread TCPserverThread(&TCPServer);
					TCPserverThread.detach();
					std::thread UDPserverThread(&UDPServer);
					UDPserverThread.detach();
					Run(window);
				}
				if (selectedOption == 1) // Select Client/Join
				{
					keypress = true;
					Run(window);
				}
				if (selectedOption == 2)
					return false;
			}

			sf::Vector2f pos{ 20,100 };
			text.setCharacterSize(20);
			text.setString("Cars Multiplayer (the game)");
			text.setStyle(sf::Text::Bold);
			text.setPosition(pos);
			text.setFillColor(sf::Color::White);
			window.draw(text);

			int count{ 0 };
			for (auto p : choices)
			{
				pos.y += 90;

				text.setString(p);
				text.setPosition(pos);
				text.setStyle(sf::Text::Regular);
				if (count == selectedOption)
					text.setFillColor(sf::Color::Red);
				else
					text.setFillColor(sf::Color::White);

				window.draw(text);
				count++;
			}
			text.setString("press 'space' to select");
			text.setPosition(10, 10);
			text.setFillColor(sf::Color::White);
			text.setCharacterSize(10);
			window.draw(text);
		}
		window.display();
	}
	return 0;
}

// GAME AND NETWORK CONNECTION
void Run(sf::RenderWindow& window)
{
	Game newGame;

	// ------- TCP SERVER CONNECT ------- 
	std::shared_ptr<sf::TcpSocket> TCPsocket = std::make_shared<sf::TcpSocket>();

	if (TCPsocket->connect(sf::IpAddress::getLocalAddress(), TCPPORT) != sf::Socket::Done)
	{
		std::stringstream ss;
		ss << "!!! Client Failed to connect to TCP socket -- Port is: " << TCPPORT << std::endl;
		std::cout << ss.str();
		return;
	}

	// ------- UDP SERVER CONNECT ------- 
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

	// ------- LAUNCH CLIENT REC ------- 
	std::list<sf::Packet> queue;
	ClientReceiver clientReceiver(&UDPsocket, TCPsocket, queue);
	std::thread receiverThread(&ClientReceiver::RecLoop, clientReceiver);
	receiverThread.detach();

	// -------  SEND AND REC MESSAGE FOR PLAYER SETUP ------- 
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
		while (queue.empty()) // there's probably a better way to do this
		{
		}
	}

	std::cerr << "pack recevied\n";
	sf::Packet recPack = queue.front(); //this might require a mutex? not causing issues so far
	ClientInfo recInfo;
	recPack >> setupInfo;
	if (setupInfo.type == EMsgType::Setup)
		queue.pop_front();
	else
	{
		std::cerr << "!!! Message is not setup pack, returing\n";
		return;
	}

	
	// ------- GAME INITIALISATION ------- 
	window.setFramerateLimit(60);
	int localPlayer{ setupInfo.localID };

	sf::Texture tBackground, tCar;
	tBackground.loadFromFile("images/background.png");
	tCar.loadFromFile("images/car.png");
	tBackground.setSmooth(true);
	tCar.setSmooth(true);
	sf::Sprite sBackground(tBackground), sCar(tCar);
	sBackground.scale(2, 2);
	sf::Font font;
	if (!font.loadFromFile("data/unispace bd.ttf"))
	{
		std::cout << "ERROR: cannot find font\n";
		return;
	}
	sf::Text playerName;
	playerName.setFont(font);
	playerName.setCharacterSize(15);
	playerName.setOutlineThickness(3);
	sCar.setOrigin(22, 22); // centre of cars sprite
	float collisionVal = 22; // used for collision checks, value dictates the box size
	const int players = 5;
	std::vector<Car> car(players, Car{});

	for (int i = 0; i < car.size(); i++)
	{
		car[i].name = newGame.names[i];
	}

	ClientInfo myInfo;
	myInfo.ip = "localhost";
	myInfo.port = ClientPort;
	myInfo.localID = localPlayer;
	myInfo.type = EMsgType::positionUpdate;

	// Starting positions
	for (int i = 0; i < players; i++)
	{
		car[i].pos.x = (300 + i * 50);
		car[i].pos.y = (1700 + i * 80);
		//car[i].speed = 7 + ((float)i / 5);
	}
	float offsetX = 0, offsetY = 0; // Camera position

	// GAME RUNNING
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		bool Up = 0, Right = 0, Down = 0, Left = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    Up = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Right = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  Down = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  Left = 1;

		//LOCAL UPDATES
		auto temppos = car[localPlayer].pos;
		if (window.hasFocus())
			car[localPlayer].Move();

		for (int i = 0; i < players; i++)
		{
			if (i != localPlayer) //
			{
			//	car[i].FindTarget(newGame.points);
			}
			car[i].Update();
		}

		//collision
		for (int i = 0; i < players; i++)
		{
			for (int j = 0; j < players; j++)
			{
				if (i == j)
				{
					break;
				}
				int dx = 0, dy = 0;
				while (dx * dx + dy * dy < 4 * collisionVal * collisionVal)
				{
					car[i].pos.x += dx / 10.0;
					car[i].pos.x += dy / 10.0;
					car[j].pos.x -= dx / 10.0;
					car[j].pos.y -= dy / 10.0;
					dx = car[i].pos.x - car[j].pos.x;
					dy = car[i].pos.y - car[j].pos.y;
					if (!dx && !dy) break;
				}
			}
		}

		//SEND TO SERVER
		if (myInfo.pos != temppos)
		{
			myInfo.pos = car[localPlayer].pos;
			myInfo.angle = car[localPlayer].angle;

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

			if (recInfo.localID != localPlayer && recInfo.localID <= players) // check to keep the player updating, also catches bad data that could cause a crash
			{
				car[recInfo.localID].pos = recInfo.pos;
				car[recInfo.localID].angle = recInfo.angle;
				//car[recInfo.localID].speed = recInfo.speed;
			}
		}

		// Step 3: Render
		window.clear(sf::Color::White);

		// Camera contained within map
		if (car[localPlayer].pos.x > 400)
			offsetX = car[localPlayer].pos.x - 400;
		if (car[localPlayer].pos.y > 400)
			offsetY = car[localPlayer].pos.y - 400;
		if (car[localPlayer].pos.x > 2480)
			offsetX -= car[localPlayer].pos.x - 2480;
		if (car[localPlayer].pos.y > 3248)
			offsetY -= car[localPlayer].pos.y - 3248;

		//player doesnt go out of bounds
		if (car[localPlayer].pos.x < 20)
			car[localPlayer].pos.x = 20;
		if (car[localPlayer].pos.y < 20)
			car[localPlayer].pos.y = 20;
		if (car[localPlayer].pos.x > 2860) // vals to change based on screen size is possible
			car[localPlayer].pos.x = 2860;
		if (car[localPlayer].pos.y > 3628)
			car[localPlayer].pos.y = 3628;

		sBackground.setPosition(-offsetX, -offsetY);
		window.draw(sBackground);
		for (int i = 0; i < players; i++)
		{
			sCar.setPosition(car[i].pos.x - offsetX, car[i].pos.y - offsetY);
			sCar.setRotation(car[i].angle * 180 / 3.141593);
			sCar.setColor(newGame.colours[i]);
			window.draw(sCar);
			playerName.setString(car[i].name);
			playerName.setPosition(car[i].pos.x - offsetX - 20, car[i].pos.y - offsetY + 25);
			window.draw(playerName);
		}

		window.display();
	}
}
