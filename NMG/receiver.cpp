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

void Receiver::recvLoop()
{
    char buffer[256];
    {
        std::stringstream ss;
        ss << "RECIEVER INFO :: " << _socket->getRemoteAddress() << ":" << _socket->getRemotePort() << std::endl;
        std::cout << ss.str();
    }
    while(1)
    {
        std::memset(buffer, 0, 256);
        std::size_t received;

        
        auto status = _socket->receive(buffer, 256, received); //receive a message here
        if (status == sf::Socket::Done)
        {
            // append the buffer to the packet
            sf::Packet receivedPacket;
            receivedPacket.append(buffer, received);
            {
                std::stringstream ss;
                std::cout << "Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
                std::cout << ss.str();
            }
            std::cout << "QUEUE HAS RECIVED THE INFO\n";
            _queue.push(receivedPacket);
        }
        else if (status == sf::Socket::Disconnected)
        {
            // handle socket disconnection
            std::cout << "RecvLoop Dropped";
            break;
        }
    }
}