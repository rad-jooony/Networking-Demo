#include "Game.h"
#include "Car.h"
#include <iostream>

void Game::Run(sf::RenderWindow& window)
{
	std::cerr << "balls\n";
	// ****************************************
	// Initialise - Setting seed and Render Window
	srand(time(NULL));
	//sf::RenderWindow window(sf::VideoMode(1000, 800), "Car Racing Game!");
	window.setFramerateLimit(60);
	sf::Texture tBackground, tCar;
	tBackground.loadFromFile("images/background.png");
	tCar.loadFromFile("images/car.png");

	tBackground.setSmooth(true);
	tCar.setSmooth(true);

	sf::Sprite sBackground(tBackground), sCar(tCar);
	sBackground.scale(2, 2);
	sCar.setOrigin(22, 22); // centre of cars sprite
	float CollisionVal = 22; // used for collision checks, value dictates the box size
	const int Players = 5;
	Car car[Players]; // Make array of 5 cars
	int localUser{ 0 };
	// There would need to be a way to figure out what car is the local user's car

	sf::Color colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Magenta, sf::Color::Blue, sf::Color::White };

	// Starting positions
	for (int i = 0; i < Players; i++)
	{
		car[i].x = (300 + i * 50);
		car[i].y = (1700 + i * 80);
		car[i].speed = 7 + ((float)i / 5); // See if this actually does anything
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
		for (int i = 1; i < Players; i++) car[i].FindTarget(Game::points);
		for (int i = 0; i < Players; i++) car[i].Update();

		//collision
		for (int i = 0; i < Players; i++)
		{
			for (int j = 0; j < Players; j++)
			{
				if (i == j)
				{
					break;
				}
				int dx = 0, dy = 0;
				while (dx * dx + dy * dy < 4 * CollisionVal * CollisionVal)
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
		// TODO: Stay within the limit of the map.
		// TODO: Don't show white at bottom/right.
		if (car[localUser].x > 400) 
			offsetX = car[localUser].x - 400;
		if (car[localUser].y > 400)
			offsetY = car[localUser].y - 400;
		if (car[localUser].x > 2482)
			offsetX -= car[localUser].x - 2482; ///These values still need tweaking
		if (car[localUser].y > 3255)
			offsetY -= car[localUser].y - 3255;

		sBackground.setPosition(-offsetX, -offsetY);
		window.draw(sBackground);
		for (int i = 0; i < Players; i++)
		{
			sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
			sCar.setRotation(car[i].angle * 180 / 3.141593);
			sCar.setColor(colors[i]);
			window.draw(sCar);
		}
		// DEBUG
		if (1)
		{
			for (int i = 0; 8 > i; i++)
			{
				sf::RectangleShape checkRec(sf::Vector2f(10, 10));
				checkRec.setPosition(sf::Vector2f(points[i].x - offsetX, points[i].y - offsetY));
				window.draw(checkRec);

			}

			sf::Font font;
			if (!font.loadFromFile("data/unispace bd.ttf"))
			{
				std::cout << "ERROR: cannot find font\n";
				return;
			}
			sf::Text text;
			text.setFont(font);
			auto X = std::to_string(car[localUser].x) + "  " + std::to_string(car[localUser].y);
			text.setString(X);
			window.draw(text);
		}
		sf::RectangleShape checkRec(sf::Vector2f(1, 1));
		checkRec.setPosition(sf::Vector2f(300 - offsetX, 300 - offsetY));
		window.draw(checkRec);


		window.display();
	}
	return;
}