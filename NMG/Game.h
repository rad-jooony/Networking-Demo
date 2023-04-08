#pragma once
#include "FrontEnd.h"
enum class GameState
{
	eMenu,
	ePlaying,
	eScore
};

class Game
{
	GameState _gameState{ GameState::eMenu };
	void WindowRender(sf::RenderWindow& window, sf::Clock& timer);

public:
	void Run(sf::RenderWindow& window);

};