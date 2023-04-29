#include "receiver.h"
#include "util.h"
#include <iostream>
#include <thread>

// Part of the TCP Socket. Recives messages. Used by both Client and Server (i think)

Receiver::Receiver(std::shared_ptr<sf::TcpSocket> s,
        Queue<std::string> &q):
    socket_(s),
    queue_(q)
{
}

void Receiver::recv_loop()
{
    char buffer[256];
    {
        std::stringstream ss;
        ss << socket_->getRemoteAddress() << ":" << socket_->getRemotePort() << std::endl;
        std::cout << ss.str();
    }
    while(1)
    {
        std::memset(buffer, 0, 256);
        std::size_t received;
        
        socket_->receive(buffer, 256, received); //receive a message here
        // TODO, socket may be closed
        {
            std::stringstream ss;
            ss << "Received: \"" << buffer << "\", " << received << " bytes." << std::endl;
            std::cout << ss.str();
        }
        queue_.push(std::string(buffer));
    }
}

