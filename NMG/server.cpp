#include "server.h"
#include "queue.h"
#include "list.h"
#include "receiver.h"
#include "util.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>


void server()
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
