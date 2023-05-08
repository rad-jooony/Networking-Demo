#pragma once 
#include <list>
#include "Util.h"


// since the UDP server does not need an accepter, 
// I can create the accepter class here
class Accepter {
private:
    Queue<sf::Packet>& _queue;
    List<std::shared_ptr<sf::TcpSocket>>& _socket;

public:
    Accepter(Queue<sf::Packet>& q, List<std::shared_ptr<sf::TcpSocket>>& s);
    void operator()();
};

void TCPserver();