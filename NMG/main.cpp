/*
Networking and Multiplayer Demo

Things to note for moving to the Linux machines
Use Code::Blocks
SetFillColor is just SetColor
Remember to include the Font

*/

#include "TCPserver.h"
#include "client.h"
#include "FrontEnd.h"
#include <iostream>
#include <thread>

int game()
{
	srand((unsigned)time(NULL));
	FrontEnd MyGame;
	MyGame.Run();
	return 0;
}

int main()
{
	while (1)
	{
		std::cout << "select what to run - Game or Server Debug - (g/s)\n";
		std::string pick;
		std::cin >> pick;

		if (pick == "g")
			game();
		if (pick == "s")
			break;
	}

	while (1)
	{
		std::cout << "select what to run - Client/UDP/TCP - (c/u/t)\n";
		std::string pick;
		std::cin >> pick;

		if (pick == "t")
		{
			std::thread TCPserverThread(&TCPserver);
			TCPserverThread.detach();
		}

		if (pick == "u")
			std::cerr << "UDP server not yet made";

		if (pick == "c")
			client();
	}
	return 0;
}


