#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class Car
{
	float _acc, _dec, _turnSpeed, // mostly non-changing values
		_maxSpeed; // Can change when boosting
	sf::Color _colour; //maybe make colour customisable?
	bool _ai = false;
public:

	int n; //Current checkpoint
	Car() { 
		speed = 2; angle = 0; n = 0; 		
		_maxSpeed = 12; _acc = 0.2f; _dec = 0.3; _turnSpeed = 0.08;
	}
	void Update(); // Update Car Position based on vars
	void Move(); // Car move, includes turning etc.
	void FindTarget(std::vector<sf::Vector2f> points); //For AI cars
	
	// These are public for ease of use. 
	float x, y, // Position in world
		speed, angle;// Movement speed and angle
};

