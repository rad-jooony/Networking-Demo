#pragma once 
#include <list>
#include "util.h"
#include "queue.h"
#include "list.h"

class Accepter {
private:
    Queue<std::string>& queue_;
    List<std::shared_ptr<sf::TcpSocket>>& socket_;

public:
    Accepter(Queue<std::string>& q, List<std::shared_ptr<sf::TcpSocket>>& s);
    void operator()();
};

void server();