#pragma once
#include "Util.h"
#include <string>

// This is the info that will be sent to the server and then sent to each client to upadte the game state

class ClientInfo
{
public:
	unsigned short id; // This will be the player's car number
	std::string username;

	float x,
		y,
		speed,
		angle; // car info

	friend sf::Packet& operator<<(sf::Packet& packet, const ClientInfo& client) // overide the << function for packets for ClientInfo
	{
		packet << client.id;
		packet << client.username;
		packet << client.x;
		packet << client.y;
		packet << client.speed;
		packet << client.angle;
		
		return packet;
	}
};