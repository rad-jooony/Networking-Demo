#pragma once 
#include <list>
#include "Util.h"


// since the UDP server does not need an accepter, 
// I can create the accepter class here
class Accepter {
private:
    Queue<std::string>& queue_;
    List<std::shared_ptr<sf::TcpSocket>>& socket_;

public:
    Accepter(Queue<std::string>& q, List<std::shared_ptr<sf::TcpSocket>>& s);
    void operator()();
};

void TCPserver();