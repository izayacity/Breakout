#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include "../include/Ball.h"
#include "../include/Paddle.h"
#include "../include/Collision.h"

class Gameplay {
private:
	const float pi = 3.14159f;
	const int gameWidth = 600;
	const int gameHeight = 800;
public:
	Ball pong;
	Paddle myPaddle;
	Paddle rightPaddle;

	sf::Font fontSansation;
	sf::Font fontArial;
	sf::Text pauseMessage;
	sf::Clock AITimer;
	sf::Text score;
	sf::Texture bgTex;    //background
	sf::RectangleShape shape;    //background

	// Define the paddles properties
	const sf::Time AITime = sf::seconds (0.1f);	
	sf::Clock clock;
	sf::Event event;

	unsigned int p1Score = -1, p2Score = 0;
	enum states { INTRO, MODE1, MODE2, MODE1playing, MODE2playing, P1WON, P1LOST };
	int gameState = INTRO;
	int isSlow = 0;

	int init ();
	void restart ();
	void updateScore ();
	int selectMode (sf::RenderWindow& window);
	void gameMode1 ();
};