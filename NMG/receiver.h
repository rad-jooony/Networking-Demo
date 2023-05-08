#pragma once

// Reciever thread gets information from other players.
// It does this in a loop. 

// both TCP and UDP need a receiver so this is its own file

#include "queue.h"
#include <SFML/Network.hpp>

class Receiver {
    public:
        Receiver(std::shared_ptr<sf::TcpSocket> s, Queue<sf::Packet>& queue);
        void recvLoop();
        sf::Packet getPacket() { return _queue.pop(); }
    private:
        std::shared_ptr<sf::TcpSocket> _socket;
        Queue<sf::Packet> &_queue;
};
