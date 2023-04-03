#include <iostream>

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
int main()
{
	int pick{ 0 };

	while (1)
	{
		std::printf("Do you want to a client or server (c/s)\n");
		std::cin >> pick;
		if (pick == 1)
		{
			std::printf("crabs\n");
		}
		else if (pick == 2)
		{
			std::printf("slabs\n");
		}
		else
		{
			std::printf("wacko\n");
			pick = 0;
		}
	}
	return 0;
};