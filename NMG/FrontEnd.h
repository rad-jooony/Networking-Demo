#pragma once
#include <SFML/Graphics.hpp>

// Front end is a class that's used to decide whats presented to the screen, i.e main menu or game
// Inadvertantly this means it controles the game state

class FrontEnd
{
private:

public:
	void Run(); //Runs the game
	void Connect(); // launches a connection menu (WIP)
	bool MainWindow(sf::RenderWindow& window); //Runs the main menu
};

