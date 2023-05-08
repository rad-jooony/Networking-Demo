#pragma once
#include "FrontEnd.h"

class Game
{
	int _cars{ 5 };
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
		"Bobcut"
	};

	void Run(sf::RenderWindow& window);
};

