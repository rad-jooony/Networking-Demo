#pragma once
#include "util.h"

enum MessageClass
{
    EClientInfo,
    EPosition,
    E
};

struct ClientInfo 
{
    sf::IpAddress ip;
    unsigned short port;
    sf::Vector2f pos;
};