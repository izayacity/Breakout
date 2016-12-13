#pragma once
#include <SFML/Graphics.hpp>

class Paddle {
public:
	sf::RectangleShape paddle;
	sf::Vector2f paddleSize;
	const float paddleSpeed;

	Paddle () : paddleSize(100.0f, 25.0f), paddleSpeed(400.f){
		paddle.setSize (paddleSize - sf::Vector2f (3, 3));
		paddle.setOutlineThickness (1);
		paddle.setOutlineColor (sf::Color::White);
		paddle.setFillColor (sf::Color (128, 0, 128));
		paddle.setOrigin (paddleSize / 2.f);
	}

	sf::Vector2f getPosition () {
		return paddle.getPosition ();
	}
};