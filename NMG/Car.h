#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class Car
{
	float _acc, 
		_dec,
		_turnSpeed, 
		_maxSpeed;
	sf::Color _colour;
	bool _ai;
public:

	int n; //Current checkpoint
	Car() { 
		speed = 2;
		angle = 0;
		n = 0; 		
		_maxSpeed = 12;
		_acc = 0.2f;
		_dec = 0.3;
		_turnSpeed = 0.08;
		_ai = false;
	}
	std::string name; // for player names

	// These are public for ease of use, though usually these should be private
	float x,
		y, // Position in world
		speed,
		angle;// Movement speed and angle

	void Update(); // Update Car Position based on vars
	void Move(); // Car move, includes turning etc.
	void FindTarget(std::vector<sf::Vector2f> points); //For AI cars
	bool isAI() { return _ai; }
};

