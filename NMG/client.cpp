#include <iostream>
#include <thread>
#include "Receiver.h"
#include "Util.h"

void client() // No longer used, being kept for reference
{
    std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();

    sf::Socket::Status status = socket->connect(sf::IpAddress::getLocalAddress(), TCP_PORT);
    if (status != sf::Socket::Done) 
    {
        std::cerr << "TCP Client Error connecting";
        return;
    }
    std::cout << "TCP Client Connected\n";

    Queue<sf::Packet> queue;
    
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue); //launch a receiver thread to receive messages from the server.
    std::thread recvThread(&Receiver::recvLoop, receiver);
   
    std::string s; // or create a packet
    while (1)
    {
        std::getline(std::cin, s);
        std::cout << "TCP Sending: \"" << s << "\"" << std::endl;
        
        socket->send(s.c_str(), s.size()); // send messages to the server
        // TODO check that this works
    }
    recvThread.join();
    return;
}
