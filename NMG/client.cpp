#include "queue.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

int client()
{
    std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
    sf::Socket::Status status = socket->connect(sf::IpAddress::getLocalAddress(), PORT);
    if (status != sf::Socket::Done) {
        return 1;
    }
    std::cout << "Connected\n";
    Queue<std::string> queue;
    // TODO launch a receiver thread to receive messages from the server.
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue);
    std::thread recvThread(&Receiver::recv_loop, receiver);
    std::string s = "";
    while (1)
    {
        std::getline(std::cin, s);
        std::cout << "Sending: \"" << s << "\"" << std::endl;
        // TODO send messages to the server
        socket->send(s.c_str(), s.size());
        // TODO check that is works
    }
    recvThread.join();
    return 0;
}
