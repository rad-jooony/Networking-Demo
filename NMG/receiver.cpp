#include "receiver.h"
#include "util.h"
#include <iostream>
#include <thread>

// Part of the TCP Socket. Recives messages. Used by both Client and Server (i think)

Receiver::Receiver(std::shared_ptr<sf::TcpSocket> s,
        Queue<sf::Packet> &q):
    _socket(s),
    _queue(q)
{}

sf::Packet transferPacket(Queue<sf::Packet>& q) //hands the packet information and pops the queue
{
    return q.pop();
}

void Receiver::recvLoop()
{
    char buffer[256];
    {
        std::stringstream ss;
        ss << _socket->getRemoteAddress() << ":" << _socket->getRemotePort() << std::endl;
        std::cout << ss.str();
    }
    while(1)
    {
        std::memset(buffer, 0, 256);
        std::size_t received;
        
        _socket->receive(buffer, 256, received); //receive a message here
        // TODO, socket may be closed
        {
            std::stringstream ss;
            ss << "Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
            std::cout << ss.str();
        }
        sf::Packet pack;
        pack << buffer;
        _queue.push(pack);
    }
}