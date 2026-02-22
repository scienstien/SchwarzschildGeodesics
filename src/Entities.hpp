#pragma once
#include <SFML/System/Vector2.hpp>

struct Ray {
	double r = 0.0;
	double phi = 0.0;
	double pr = 0.0;
	double pphi = 0.0;
	double L = 0.0; 


	bool captured = false;

	std::vector<sf::Vector2f> trail;
};

struct BlackHole
{
	sf::Vector2f pos;
	float mass = 0.0f;
};