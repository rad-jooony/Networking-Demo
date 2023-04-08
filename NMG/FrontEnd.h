#pragma once
#include <SFML/Graphics.hpp>

class FrontEnd
{
private:

public:
	void Run(); //Runs the game
	void Connect(); // launches a connection menu (WIP)
	bool MainWindow(sf::RenderWindow& window); //Runs the main menu
};

