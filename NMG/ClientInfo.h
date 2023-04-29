#pragma once
#include "Util.h"
#include <string>

//THis is the info that will be sent to the server and then sent to each client to upadte the game state

class ClientInfo
{
public:
	unsigned short id; // This will be the player's car number
	std::string username; // This is the player's username and will be visable below their car
	sf::IpAddress IP;
	unsigned short port;

	float x, y, speed, angle; // car info
	 
	//TODO fill client info?

private:
};