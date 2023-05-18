#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

// includes game variables and car data

class Game
{
public:
	const int num = 8; //checkpoints
	std::vector<sf::Vector2f> points //checkpoint locations
	{
		sf::Vector2f(300, 610),
		sf::Vector2f(1270,430),
		sf::Vector2f(1380,2380),
		sf::Vector2f(1900,2460),
		sf::Vector2f(1970,1700),
		sf::Vector2f(2550,1680),
		sf::Vector2f(2560,3150),
		sf::Vector2f(500, 3300)
	};

	std::vector<std::string> names // default names for each player
	{
		"Ace",
		"Hero",
		"Panther",
		"King",
		"Bangs"
	};

	std::vector<sf::Color> colours
	{
		sf::Color::Blue,
		sf::Color::Magenta,
		sf::Color::Red,
		sf::Color::White,
		sf::Color::Yellow
	};
};

class Car
{
	float _acc,
		_dec,
		_turnSpeed,
		_maxSpeed;
	sf::Color _colour;
	bool _ai;
public:

	int n; //Current checkpoint
	Car() {
		speed = 0;
		angle = 0;
		n = 0;
		_maxSpeed = 12;
		_acc = 0.2f;
		_dec = 0.3;
		_turnSpeed = 0.08;
		_ai = false;
	}
	std::string name; // for player names

	// These are public for ease of use, though usually these should be private
	sf::Vector2f pos;
	float speed,
		angle;

	void Update(); // Update Car Position based on vars
	void Move(); // Car move, includes turning etc.
	void FindTarget(std::vector<sf::Vector2f> points); //For AI cars
	bool isAI() { return _ai; }
};