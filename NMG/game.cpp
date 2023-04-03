#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <cmath>
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

const int num = 8; //checkpoints
// TODO: use checkpoint to make sure we are on the track.
// Slow speed when not on the track.
int points[num][2] = { 300, 610,
    1270,430,
    1380,2380,
    1900,2460,
    1970,1700,
    2550,1680,
    2560,3150,
    500, 3300 };



struct Car
{
    float x, y, //Position in world
        speed, angle;
    int n; //Current checkpoint
    Car() { speed = 2; angle = 0; n = 0; }
    void move()
    {
        x += sin(angle) * speed;
        y -= cos(angle) * speed;
    }
    void findTarget()
    {
        float tx = points[n][0];
        float ty = points[n][1];
        float beta = angle - atan2(tx - x, -ty + y);
        if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
        // Check if passed a checkpoint
        //if ((x - tx) * (x - tx) + (y - ty) * (y - ty) < 25 * 25)
        if (pow(x - tx,2) + pow(y - ty, 2) < 625)
            n = (n + 1) % num; // TODO: simplify
    }
};

int Game()
{
    // ****************************************
    // Initialise
    srand(time(NULL));
    sf::RenderWindow app(sf::VideoMode(1000,800), "Car Racing Game!");
    app.setFramerateLimit(60);
    sf::Texture tBackground, tCar;
    tBackground.loadFromFile("images/background.png");
    tCar.loadFromFile("images/car.png");
    tBackground.setSmooth(true);
    tCar.setSmooth(true);
    sf::Sprite sBackground(tBackground), sCar(tCar);
    sBackground.scale(2, 2);
    sCar.setOrigin(22, 22); // centre of cars sprite
    float CollisionVal = 22; // used for collision checks, value dictates the box size
    const int Player = 5;
    Car car[Player];
    sf::Color colors[5] = { sf::Color::Red, sf::Color::Green, sf::Color::Magenta, sf::Color::Blue, sf::Color::White };

    // Starting positions
    for (int i = 0; i < Player; i++)
    {
        car[i].x = 300 + i * 50;
        car[i].y = 1700 + i * 80;
        car[i].speed = 7 + ((float)i / 5);
    }
    float speed = 0, angle = 0;
    float maxSpeed = 12.0;
    float acc = 0.2, dec = 0.3;
    float turnSpeed = 0.08;
    int offsetX = 0, offsetY = 300; // Camera position possibly

    // ****************************************
    // Loop

    while (app.isOpen())
    {
        sf::Event e;
        while (app.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                app.close();
        }

        // Step 1: user input
        bool Up = 0, Right = 0, Down = 0, Left = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    Up = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Right = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  Down = 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  Left = 1;
        
        // Step 2: update
        //car movement
        if (Up && speed < maxSpeed)
        {
            if (speed < 0)  speed += dec;
            else  speed += acc;
        }
        if (Down && speed > -maxSpeed)
        {
            if (speed > 0) speed -= dec;
            else  speed -= acc;
        }
        if (!Up && !Down)
        {
            if (speed - dec > 0) speed -= dec;
            else if (speed + dec < 0) speed += dec;
            else speed = 0;
        }
        if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
        if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;
        car[0].speed = speed;
        car[0].angle = angle;
        for (int i = 0; i < Player; i++) car[i].move();
        for (int i = 1; i < Player; i++) car[i].findTarget();
        //collision
        for (int i = 0; i < Player; i++)
        {
            for (int j = 0; j < Player; j++)
            {
                if (i == j)
                {
                    break;
                }
                int dx = 0, dy = 0;
                while (dx * dx + dy * dy < 4 * CollisionVal * CollisionVal)
                {
                    car[i].x += dx / 10.0;
                    car[i].x += dy / 10.0;
                    car[j].x -= dx / 10.0;
                    car[j].y -= dy / 10.0;
                    dx = car[i].x - car[j].x;
                    dy = car[i].y - car[j].y;
                    if (!dx && !dy) break;
                }
            }
        }
        // Step 3: Render
        app.clear(sf::Color::White);
        // TODO: Stay within the limit of the map.
        // TODO: Don't show white at bottom/right.
        if (car[0].x > 320) offsetX = car[0].x - 320;
        if (car[0].y > 240) offsetY = car[0].y - 240;
        sBackground.setPosition(-offsetX, -offsetY);
        app.draw(sBackground);
        for (int i = 0; i < Player; i++)
        {
            sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
            sCar.setRotation(car[i].angle * 180 / 3.141593);
            sCar.setColor(colors[i]);
            app.draw(sCar);
        }
        app.display();
    }
    return 0;
}
