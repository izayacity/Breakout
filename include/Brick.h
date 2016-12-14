#pragma once
#include <SFML/Graphics.hpp>
#include <assert.h>

class Brick {
public:
	sf::RectangleShape brick;
	sf::Vector2f brickSize;
	sf::Texture brokeTex;
	int type;
	int life;

	Brick () : brickSize(sf::Vector2f (100.0f, 25.0f)) {		
	}
	// normal brick, grey
	void init1 (sf::Texture *brickTexture) {
		type = 0;
		life = 1;		
		brick.setFillColor (sf::Color (131, 60, 30));
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setOrigin (brickSize / 2.f);
		brick.setTexture (brickTexture);
	}
	// hard brick, brown
	void init2 (sf::Texture *brickTexture) {
		type = 1;
		life = 2;
		brick.setFillColor (sf::Color (195, 187, 168));
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setOrigin (brickSize / 2.f);
		brick.setTexture (brickTexture);
		assert (brokeTex.loadFromFile ("resources/crack.jpg"));
		brokeTex.setSmooth (true);
	}
	// invisible hard brick, no color
	void init3 () {
		type = 2;
		life = 2;
		brick.setSize (brickSize);
		brick.setOrigin (brickSize / 2.f);
		assert (brokeTex.loadFromFile ("resources/crack.jpg"));
		brokeTex.setSmooth (true);

	}
	// invincible brick, dark grey
	void init4 (sf::Texture *brickTexture) {
		type = 3;
		life = ((unsigned int)-1) >> 1;
		brick.setFillColor (sf::Color (51, 51, 51));
		brick.setSize (brickSize);
		brick.setOutlineThickness (1);
		brick.setOutlineColor (sf::Color::White);
		brick.setOrigin (brickSize / 2.f);
		brick.setTexture (brickTexture);
	}

	sf::Vector2f getPosition () {
		return brick.getPosition ();
	}
};