#pragma once
#include "util.h"


// look into enum class?


struct ClientInfo
{
private:
public:
	sf::IpAddress ip;
	unsigned short port;
	sf::Vector2f pos;

	friend sf::Packet& operator<<(sf::Packet& packet, const ClientInfo& client) // overide the << function for packets for ClientInfo
	{
		std::string ipString = client.ip.toString();
		packet << ipString;
		packet << client.port;
		packet << client.pos.x;
		packet << client.pos.y;

		return packet;
	}
	friend sf::Packet& operator>>(sf::Packet& packet, ClientInfo& client) // overide the >> function for packets for ClientInfo
	{
		std::string ipString;
		packet >> ipString;
		client.ip = sf::IpAddress(ipString);
		packet >> client.port;
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