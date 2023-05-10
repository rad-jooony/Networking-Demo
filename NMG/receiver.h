#pragma once

// A receiver is needed by each 

#include <SFML/Network.hpp>

class Receiver 
{


public:
	Receiver(std::shared_ptr<sf::TcpSocket> s, Queue<sf::Packet>& queue);



};