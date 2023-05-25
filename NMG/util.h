#pragma once

// Utility file. Using this for various header files and includes that are used in most files

#define UDPPORT 54000
#define CLIENTPORT 54001
#define TCPPORT 54002

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <iostream>
#include <thread>
#include "Messages.h"

/*
std::stringstream ss;
ss << "" << std::endl;
std::cout << ss.str();
*/