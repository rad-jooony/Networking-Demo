#include "TCPserver.h"
#include "receiver.h"
#include "util.h"
#include <iostream>
#include <thread>

Accepter::Accepter(Queue<std::string>& q, List<std::shared_ptr<sf::TcpSocket>>& s) :
    queue_(q),
    socket_(s)
{}

void Accepter::operator()()//double bracket makes this a function
{
    sf::TcpListener listener;
    if (listener.listen(TCP_PORT) != sf::Socket::Done)
    {
        std::cerr << "TCP Error listening to accepter";
        return;
    }
    std::cout << "TCP Bound to port : " << TCP_PORT << "\n";

    while (true)
    {
        std::shared_ptr<sf::TcpSocket> socket = std::make_shared<sf::TcpSocket>();
        if (listener.accept(*socket) != sf::Socket::Done)
        {
            std::cerr << "TCP Cannot accept connection";
            return;
        }
        // Now it is able to accept messages
        socket_.push(socket); //add this socket to the list of sockets

        std::stringstream ss;
        ss << "Accepted a connection from: " << socket->getRemoteAddress() << ":" << socket->getRemotePort() << std::endl;
        std::cout << ss.str();


        std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(socket, queue_);

        // launch a thread to receive with the receiver
        std::thread(&Receiver::recv_loop, receiver).detach(); // thread lauches detached, meaning it doesnt need to wait to catch up
    }
}


void TCPserver()
{
	std::cout << "launching Server\n";
	Queue<std::string> queue; ///Server creates a queue, what is this for? im assuming messages since the type is string
	List<std::shared_ptr<sf::TcpSocket>> sockets; /// a list of shared pointers to sockets, assuming this allows connection between multiple clients?
	
	std::thread accThread(Accepter(queue, sockets)); // an accepter thread is launched. This creates a thread that will accept incoming connections.
	
	while (1)
	{
		std::string s = queue.pop();
		std::cout << "Main read: \"" << s << "\"\n";
		// TODO send to all in sockets. Be careful to synchronise.
		auto send_to_one = [&s](std::shared_ptr<sf::TcpSocket> socket) {
			socket->send(s.c_str(), s.size());
			// TODO: check that this is successfull
		};
		sockets.for_each(send_to_one);

		/// Im assuiming this is sending each recived message to every client?
	}
	return;
}
