#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include "queue.h"
#include "receiver.h"
#include "util.h"

#include "server.h"
#include "client.h"

/*
	in main:
	ask the user if they want a server or client

	Later:

	UDP send bcast message
	set udp socket to non blocking
	wait a bit
	try to receive
	if there is a server, the connect to it
	else spawn a server, wait a bit then connect
*/

int other()
{
	while (1)
	{
		std::cout << "select what to run - Client/UDP/TCP - (c/u/t)\n";
		std::string pick;
		std::cin >> pick;

		if (pick == "t")
		{
			std::thread TCPserverThread(&server);
			TCPserverThread.detach();
		}

		if (pick == "u")
			std::cerr << "UDP server not yet made";

		if (pick == "c")
			client();
	}
	return 0;
}