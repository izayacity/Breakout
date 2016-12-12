#pragma once
#include <SFML/Graphics.hpp>
#include <assert.h>

class Brick {
public:
	sf::RectangleShape brick;
	sf::Vector2f brickSize;
	sf::Texture brickTex;
	sf::Texture brokeTex;
	int type;
	int life;

	Brick () : brickSize(sf::Vector2f (100.0f, 25.0f)) {
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setOrigin (brickSize / 2.f);		
		assert (brickTex.loadFromFile ("resources/brick.jpg"));
		brickTex.setSmooth (true);
		assert (brokeTex.loadFromFile ("resources/crack.jpg"));
		brokeTex.setSmooth (true);		
	}

	void init1 () {
		type = 0;
		life = 1;		
		brick.setFillColor (sf::Color (131, 60, 30));
		brick.setTexture (&brickTex);
	}

	void init2 () {
		type = 1;
		life = 2;
		brick.setFillColor (sf::Color (195, 187, 168));
		brick.setTexture (&brickTex);
	}

	sf::Vector2f getPosition () {
		return brick.getPosition ();
	}
};