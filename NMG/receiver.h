#ifndef RECEIVER_HPP
#define RECEIVER_HPP

// Reciever thread gets information from other players.
// It does this in a loop. 

#include "queue.h"
#include <SFML/Network.hpp>

class Receiver {
    public:
        Receiver(std::shared_ptr<sf::TcpSocket> s, Queue<std::string>& queue);
        void recv_loop();
    private:
        std::shared_ptr<sf::TcpSocket> socket_;
        Queue<std::string> &queue_;
};
#endif
