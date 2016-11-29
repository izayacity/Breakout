#pragma once
#include <SFML/Graphics.hpp>

class Brick {
public:
	sf::RectangleShape brick;
	sf::Vector2f brickSize;

	Brick () {
		brickSize = sf::Vector2f (100.0f, 25.0f);
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setFillColor (sf::Color (131, 60, 30));
		brick.setOrigin (brickSize / 2.f);
	}

	sf::Vector2f getPosition () {
		return brick.getPosition ();
	}
};