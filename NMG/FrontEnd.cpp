#include "FrontEnd.h"
#include "Game.h"
#include <iostream>

void FrontEnd::Run()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Cars! The GAME?!?! WOWOWEEE!!");

	while (MainWindow(window)) //This acts as the main menu. 
	{
		Game game;
		game.Run(window);
	}
}

void FrontEnd::Connect()
{
	std::cerr << "connection menu (wip)";
}

//MAIN MENU
bool FrontEnd::MainWindow(sf::RenderWindow& window)
{
	sf::Font font;
	if (!font.loadFromFile("data/unispace bd.ttf"))
	{
		std::cout << "ERROR: cannot find font\n";
		return false;
	}

	// establish options that can be picked
	std::vector<std::string> choices;
	choices.push_back("Host Game");
	choices.push_back("Join Game");
	choices.push_back("Quit");

	// options are slected from 0 to array size
	int selectedOption{ 0 };

	// Initialise text
	sf::Text text;
	text.setFont(font);
	bool keypress{ false }; // used to keep from option skipping (makes no difference with 2 or less options)

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				return false;
			default:
				break;
			}
		}
		window.clear(sf::Color::Black);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !keypress) // Select up
		{
			keypress = true;
			if (selectedOption > 0)
				selectedOption--;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !keypress) // select down
		{
			keypress = true;
			if (selectedOption < choices.size() - 1)
				selectedOption++;
		}
		if (keypress && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) // disable option switching until no keys are down
			keypress = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (selectedOption == 0)
				return true;
			if (selectedOption == 1)
				Connect();
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

		window.display();
	}
	return false;
}