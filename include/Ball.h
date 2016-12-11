#pragma once
#include <SFML/Graphics.hpp>
#include <assert.h>
#include <SFML/Audio.hpp>

class Ball : public sf::Sprite {
public:
	sf::CircleShape ball;
	const float ballRadius;
	float ballSpeed;
	float ballAngle;
	float ballAcceleration;
	sf::Texture ballTex;
	sf::SoundBuffer ballSoundBuffer;
	sf::Sound ballSound;

	Ball () : ballRadius(20.f), ballSpeed(400.0f), ballAngle(0.0f), ballAcceleration(25.0f) {
		// Create the ball
		ball.setRadius (ballRadius);
		ball.setFillColor (sf::Color::White);
		ball.setOrigin (ballRadius, ballRadius);

		// Load the texture of the ball		
		assert (ballTex.loadFromFile ("resources/ball.png"));
		assert (ballSoundBuffer.loadFromFile ("resources/ball.wav"));
		ballSound.setBuffer (ballSoundBuffer);
		ballTex.setSmooth (true);
		ball.setTexture (&ballTex);
	}

	sf::Vector2f getPosition () {
		return ball.getPosition ();
	}

	void setPosition (const sf::Vector2f& position) {
		ball.setPosition (position);
	}

	void setPosition (float offsetX, float offsetY) {
		ball.setPosition (offsetX, offsetY);
	}

	void move (float offsetX, float offsetY) {
		ball.move (offsetX, offsetY);
	}

	void move (const sf::Vector2f& offset) {
		ball.move (offset);
	}
};