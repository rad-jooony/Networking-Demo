#include "Game.h"
#include "Car.h"
#include <iostream>

#include <thread>
#include "Receiver.h"
#include "Util.h"
#include "ClientInfo.h"

void Game::Run(sf::RenderWindow& window)
{
	srand(time(NULL));
	window.setFramerateLimit(60);

	// NETWORKING (CLIENT ESTABLISHMENT)
	std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
	sf::Socket::Status status = socket->connect(sf::IpAddress::getLocalAddress(), TCP_PORT);
	if (status != sf::Socket::Done)
	{
		std::cerr << "TCP Client Error connecting";
		return;
	}
	std::cout << "Client Connected\n";
	Queue<sf::Packet> queue;
	std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue); //launch a receiver thread to receive messages from the server.
	std::thread recvThread(&Receiver::recvLoop, receiver);

	
	// GET INFO FROM SERVER // This will be the player's ID


	int localUser{ 0 };

	// GAME INITIALISATION
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
	const int players = 2;
	std::vector<Car> car(players, Car{});

	for (int i = 0; i < car.size(); i++)
	{
		car[i].name = Game::names[i];
	}
	// There would need to be a way to figure out what car is the local user's car -- Maybe?? idk

	sf::Color colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Magenta, sf::Color::Blue, sf::Color::White };

	// Starting positions
	for (int i = 0; i < players; i++)
	{
		car[i].x = (300 + i * 50);
		car[i].y = (1700 + i * 80);
		car[i].speed = 7 + ((float)i / 5);
	}

	float offsetX = 0, offsetY = 0; // Camera position

	// ****************************************
	// Loop

	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				window.close();
		}

		// Step 1: user input
		bool Up = 0, Right = 0, Down = 0, Left = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    Up = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Right = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  Down = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  Left = 1;

		// Step 2: update
		//car movement
		car[localUser].Move();

		for (int i = 0; i < players; i++)
		{
			if (car[i].isAI())
			{
				car[i].FindTarget(Game::points);
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
					car[i].x += dx / 10.0;
					car[i].x += dy / 10.0;
					car[j].x -= dx / 10.0;
					car[j].y -= dy / 10.0;
					dx = car[i].x - car[j].x;
					dy = car[i].y - car[j].y;
					if (!dx && !dy) break;
				}
			}
		}

		// Step 3: Render
		window.clear(sf::Color::White);

		// Camera contained within map
		if (car[localUser].x > 400)
			offsetX = car[localUser].x - 400;
		if (car[localUser].y > 400)
			offsetY = car[localUser].y - 400;
		if (car[localUser].x > 2480)
			offsetX -= car[localUser].x - 2480;
		if (car[localUser].y > 3248)
			offsetY -= car[localUser].y - 3248;

		//player doesnt go out of bounds
		if (car[localUser].x < 20)
			car[localUser].x = 20;
		if (car[localUser].y < 20)
			car[localUser].y = 20;
		if (car[localUser].x > 2860) // vals to change based on screen size is possible
			car[localUser].x = 2860;
		if (car[localUser].y > 3628)
			car[localUser].y = 3628;

		sBackground.setPosition(-offsetX, -offsetY);
		window.draw(sBackground);
		for (int i = 0; i < players; i++)
		{
			sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
			sCar.setRotation(car[i].angle * 180 / 3.141593);
			sCar.setColor(colors[i]);
			window.draw(sCar);
			playerName.setString(car[i].name);
			playerName.setPosition(car[i].x - offsetX - 20, car[i].y - offsetY + 25);
			window.draw(playerName);

		}

		// send server

		// recieve server

		// DEBUG SECTION
		if (1)
		{

		}

		window.display();

		sf::Packet pack;
		if (1)
		{
			//send to server
		}
		//somehow recieve from server too
	}
	recvThread.join();
	return;
}