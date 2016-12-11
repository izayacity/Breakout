#pragma once
#include <SFML/Graphics.hpp>
#include <assert.h>

class Brick {
public:
	sf::RectangleShape brick;
	sf::Vector2f brickSize;
	sf::Texture brickTex;
	int type;
	int life;

	Brick () : brickSize(sf::Vector2f (100.0f, 25.0f)) {
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setOrigin (brickSize / 2.f);		
	}

	void init1 () {
		type = 0;
		life = 1;
		assert (brickTex.loadFromFile ("resources/white.jpg"));
		brickTex.setSmooth (true);
		brick.setTexture (&brickTex);
		brick.setFillColor (sf::Color (131, 60, 30));		
	}

	void init2 () {
		type = 1;
		life = 2;
		assert (brickTex.loadFromFile ("resources/white.jpg"));
		brickTex.setSmooth (true);
		brick.setTexture (&brickTex);
		brick.setFillColor (sf::Color (195, 187, 168));
	}

	sf::Vector2f getPosition () {
		return brick.getPosition ();
	}
};