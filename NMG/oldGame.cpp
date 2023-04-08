#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <cmath>
#include <iostream>

// Most of the game and stuctures wont be put into seperate class files, as the scorpe of the game is very small

// TODO:
// use checkpoint to make sure we are on the track.
// Slow speed when not on the track.
// Main menu
// Speed boost power (charge)



// Points - or Checkpoints - are manually placed in the level. AI cars approach these checkpoints
// and they are also used to make sure players are driving correctly aswell

const int num = 8; //checkpoints
sf::Vector2f points[]
{
	sf::Vector2f(300, 610),
	sf::Vector2f(1270,430),
	sf::Vector2f(1380,2380),
	sf::Vector2f(1900,2460),
	sf::Vector2f(1970,1700),
	sf::Vector2f(2550,1680),
	sf::Vector2f(2560,3150),
	sf::Vector2f(500, 3300) };

struct Car
{
	float x, y, // Position in world
		speed, angle;
	int n; //Current checkpoint
	Car() { speed = 2; angle = 0; n = 0; }
	void move()
	{
		// if (inTrackBoxes)
		x += sin(angle) * speed;
		y -= cos(angle) * speed;
		// else
		// xy +-= sincos(angle) * speed * 0.5
	}
	void findTarget()
	{
		float tx = points[n].x;
		float ty = points[n].y;
		float beta = angle - atan2(tx - x, -ty + y);
		if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
		// Check if passed a checkpoint
		if (pow(x - tx, 2) + pow(y - ty, 2) < 625)
			n = (n + 1) % num;
	}
};

int dmain()
{
	// ****************************************
	// Initialise
	srand(time(NULL));
	sf::RenderWindow app(sf::VideoMode(1000, 800), "Car Racing Game!");
	app.setFramerateLimit(60);
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
	Car car[Players];
	sf::Color colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Magenta, sf::Color::Blue, sf::Color::White };

	// Starting positions
	for (int i = 0; i < Players; i++)
	{
		car[i].x = 300 + i * 50;
		car[i].y = 1700 + i * 80;
		car[i].speed = 7 + ((float)i / 5);
	}


	// player car vals
	float speed = 0, angle = 0;
	float maxSpeed = 12.0;
	float acc = 0.2, dec = 0.3;
	float turnSpeed = 0.08;
	int offsetX = 0, offsetY = 300; // idk what this does

	// ****************************************
	// Loop

	while (app.isOpen())
	{
		sf::Event e;
		while (app.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				app.close();
		}

		// Step 1: user input
		bool Up = 0, Right = 0, Down = 0, Left = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    Up = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Right = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  Down = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  Left = 1;

		// Step 2: update
		//car movement
		if (Up && speed < maxSpeed)
		{
			if (speed < 0)  speed += dec;
			else  speed += acc;
		}
		if (Down && speed > -maxSpeed)
		{
			if (speed > 0) speed -= dec;
			else  speed -= acc;
		}
		if (!Up && !Down)
		{
			if (speed - dec > 0) speed -= dec;
			else if (speed + dec < 0) speed += dec;
			else speed = 0;
		}
		if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
		if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;
		car[0].speed = speed;
		car[0].angle = angle;

		for (int i = 0; i < Players; i++) car[i].move();
		for (int i = 1; i < Players; i++) car[i].findTarget();

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
		app.clear(sf::Color::White);
		// TODO: Stay within the limit of the map.
		// TODO: Don't show white at bottom/right.
		if (car[0].x > 320) offsetX = car[0].x - 320;
		if (car[0].y > 240) offsetY = car[0].y - 240;
		sBackground.setPosition(-offsetX, -offsetY);
		app.draw(sBackground);
		for (int i = 0; i < Players; i++)
		{
			sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
			sCar.setRotation(car[i].angle * 180 / 3.141593);
			sCar.setColor(colors[i]);
			app.draw(sCar);
		}
		// DEBUG
		if (1)
		{
			for (int i = 0; sizeof(points)/sizeof(sf::Vector2f) > i; i++)
			{
				sf::RectangleShape checkRec(sf::Vector2f(10, 10));
				checkRec.setPosition(sf::Vector2f(points[i].x - offsetX, points[i].y - offsetY));
				app.draw(checkRec);

			}
		}
		sf::RectangleShape checkRec(sf::Vector2f(1,1));
		checkRec.setPosition(sf::Vector2f(300-offsetX, 300-offsetY));
		app.draw(checkRec);


		app.display();
	}
	return 0;
}
