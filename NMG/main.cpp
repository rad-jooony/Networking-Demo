/*
Networking and Multiplayer Demo

Things to note for moving to the Linux machines
Use Code::Blocks
SetFillColor is just SetColor
Remember to include the Font

*/

#include "FrontEnd.h"


int other()
{

	srand((unsigned)time(NULL));
	FrontEnd MyGame;
	MyGame.Run();
	return 0;
}