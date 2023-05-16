#pragma once
#include "util.h"

enum EMsgType
{
	Setup,
	positionUpdate
};

struct ClientInfo
{
private:
public:

	EMsgType type; // specifying type could be useful if there were many different elements and objects to update
	bool sendToSelf = false; 
	bool sendToOthers = true; // these two are usefull for setting up from the TCP server
	sf::IpAddress ip;
	unsigned short port;
	unsigned short localID; 
	sf::Vector2f pos;

	friend sf::Packet& operator<<(sf::Packet& packet, const ClientInfo& client) // override the << function for packets for ClientInfo
	{
		packet << client.type;
		packet << client.sendToSelf;
		packet << client.sendToOthers;

		std::string ipString = client.ip.toString();
		packet << ipString;

		packet << client.port;
		packet << client.localID;
		packet << client.pos.x;
		packet << client.pos.y;
		
		return packet;
	}
	friend sf::Packet& operator>>(sf::Packet& packet, ClientInfo& client) // override the >> function for packets for ClientInfo
	{
		int type;
		packet >> type;
		client.type = static_cast<EMsgType>(type); // method to unpack EMsgType

		packet >> client.sendToSelf;
		packet >> client.sendToOthers;

		std::string ipString;
		packet >> ipString;
		client.ip = sf::IpAddress(ipString);
		packet >> client.port;
		packet >> client.localID;
		packet >> client.pos.x;
		packet >> client.pos.y;

		return packet;
	}

	void PrintPos()
	{
		std::stringstream ss;
		ss << "X - " << pos.x
			<< " Y - " << pos.y << "\n";
		std::cout << ss.str();
	}
};