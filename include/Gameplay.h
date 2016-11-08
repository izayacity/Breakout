#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include "../include/Ball.h"
#include "../include/Paddle.h"
#include "../include/Collision.h"

class Gameplay {
private:
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
public:
	Ball pong;
	Paddle leftPaddle;
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
	enum states { INTRO, MODE1, MODE2, P1WON, P1LOST };
	int gameState = INTRO;
	int isSlow = 0;

	int init ();
	void restart ();
	void updateScore ();
	int selectMode (sf::RenderWindow& window);
	void gameMode1 ();
};