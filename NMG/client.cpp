#include "queue.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

void Cclient()
{
    std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();

    sf::Socket::Status status = socket->connect(sf::IpAddress::getLocalAddress(), PORT);
    if (status != sf::Socket::Done) 
    {
        std::cerr << "TCP Client Error connecting";
        return;
    }
    std::cout << "Connected\n";

    Queue<std::string> queue;
    
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue); //launch a receiver thread to receive messages from the server.
    std::thread recvThread(&Receiver::recv_loop, receiver);

   
    std::string s = "Maybe this is a clue to surviving bluelock????"; // or create a packet
    while (1)
    {
        std::getline(std::cin, s);
        std::cout << "Sending: \"" << s << "\"" << std::endl;
        
        socket->send(s.c_str(), s.size()); // send messages to the server
        // TODO check that this works
    }
    recvThread.join();
    return;
}
