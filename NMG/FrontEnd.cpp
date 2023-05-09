#include "FrontEnd.h"
#include "TCPserver.h"
#include "UDPserver.h"
#include "Game.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <chrono>
#include <thread>

void FrontEnd::Run()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Cars Multiplayer game");

	while (MainWindow(window)) //This acts as the main menu. 
	{
		Game game;
		game.Run(window);
	}
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !keypress) // Select down
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
			{
				std::thread TCPserverThread(&TCPserver);
				TCPserverThread.detach();
				std::thread UDPserverThread(&UDPserver);
				UDPserverThread.detach();
				return true;
			}
			if (selectedOption == 1)
				if (Connect(window))
				{
					return true;
				}
			if (selectedOption == 2)
				return false;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
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

unsigned short FrontEnd::Connect(sf::RenderWindow& window) // This is the connection menu logic
{
	sf::Font font;
	if (!font.loadFromFile("data/unispace bd.ttf"))
	{
		std::cout << "ERROR: cannot find font\n";
		return 0;
	}
	sf::Text inputText("", font, 20);
	inputText.setPosition(20, 100);
	std::string inputString;

	sf::Text message("", font, 20);
	message.setPosition(20, 300);
	bool errorMessage{ 0 };

	while (1)
	{
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::TextEntered)
			{
				if (std::isdigit(event.text.unicode) && inputString.size() < 5)
				{
					inputString += static_cast<char>(event.text.unicode); //static_cast<char> converts the unicode to a char
					inputText.setString(inputString);
				}
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Backspace && !inputString.empty())
				{
					inputString.erase(inputString.size() - 1);
					inputText.setString(inputString);
				}
				if (event.key.code == sf::Keyboard::Escape)
				{
					return 0;
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				window.clear();
				errorMessage = 1;
				message.setString("Connecting...");
				window.draw(message);
				window.display();
				//sf::TcpSocket testSocket;
				//sf::Socket::Status status = testSocket.connect(sf::IpAddress::getLocalAddress(), 4301)); // I wont check this unitl i can delete connection from the TCPserver list
				//if (status != sf::Socket::Done)
				//{
				//	message.setString("Can't Connect. Server may be offline");
				//	break;
				//}
				message.setString("Connection accepted");
				window.draw(message);
				window.display();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				return 4301;
			}

			window.clear();
			window.draw(inputText);
			if (errorMessage)
				window.draw(message);
			window.display();
		}
	}
}