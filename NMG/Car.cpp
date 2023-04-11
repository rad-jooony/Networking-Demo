#include "Car.h"
void Car::Update()
{
	// if (inTrackBoxes)
	x += sin(angle) * speed;
	y -= cos(angle) * speed;
	// else
	// xy +-= sincos(angle) * speed * 0.5
}

void Car::FindTarget(std::vector<sf::Vector2f> points) //
{
	float tx = points[n].x;
	float ty = points[n].y;
	float beta = angle - atan2(tx - x, -ty + y);
	if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
	// Check if passed a checkpoint
	if (pow(x - tx, 2) + pow(y - ty, 2) < 625)
		n = (n + 1) % points.size();
}
void Car::Move()
{
	bool Up = 0, Right = 0, Down = 0, Left = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    Up = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Right = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  Down = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  Left = 1;

	if (Up && speed < _maxSpeed)
	{
		if (speed < 0)  speed += _dec;
		else  speed += _acc;
	}
	if (Down && speed > -_maxSpeed)
	{
		if (speed > 0) speed -= _dec;
		else  speed -= _acc;
	}
	if (!Up && !Down)
	{
		if (speed - _dec > 0) speed -= _dec;
		else if (speed + _dec < 0) speed += _dec;
		else speed = 0;
	}
	if (Right && speed != 0)  angle += _turnSpeed * speed / _maxSpeed;
	if (Left && speed != 0)   angle -= _turnSpeed * speed / _maxSpeed;
}