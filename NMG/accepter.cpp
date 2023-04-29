#include "TCPserver.h"
#include "receiver.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include <thread>

// A listener, that looks out for for connections, when it does, it becomes attached to the specific port. It does not accept 
// the connection.

//Accepter::Accepter(Queue<std::string>& q, List<std::shared_ptr<sf::TcpSocket>>& s):
//    queue_(q),
//    socket_(s)
//{}
//
//void Accepter::operator()()//double bracket makes this a function
//{
//    sf::TcpListener listener;
//    if (listener.listen(TCP_PORT) != sf::Socket::Done)
//    {
//        std::cerr << "TCP Error listening to accepter";
//        return;
//    }
//    std::cout << "TCP Bound to port : " << TCP_PORT << "\n";
//
//    while (true)
//    {
//        std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
//        if (listener.accept(*socket) != sf::Socket::Done)
//        {
//            std::cerr << "TCP Cannot accept connection";
//            return;
//        }
//        // Now it is able to accept messages
//        socket_.push(socket); //add this socket to the list of sockets
//
//        std::stringstream ss;
//        ss << "Accepted a connection from: " << socket->getRemoteAddress() << ":" << socket->getRemotePort() << std::endl;
//        std::cout << ss.str();
//
//        
//        std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue_);
//        
//        // launch a thread to receive with the receiver
//        std::thread(&Receiver::recv_loop, receiver).detach(); // thread lauches detached, meaning it doesnt need to wait to catch up
//    }
//}