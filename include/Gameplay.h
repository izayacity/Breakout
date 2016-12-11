#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include "../include/Ball.h"
#include "../include/Paddle.h"
#include "../include/Collision.h"
#include "../include/Brick.h"
#include <vector>

class Gameplay {
private:
	const float pi = 3.14159f;
	const int gameWidth = 600;
	const int gameHeight = 800;

public:
	Ball pong;
	Paddle myPaddle;

	sf::Font fontHNMed;
	sf::Font fontHNM;
	sf::Font fontHNL;
	sf::Text pauseMessage;
	sf::Clock AITimer;
	sf::Text scoreText;
	sf::Text lifeText;
	sf::Texture bgTex;    //background texture
	sf::RectangleShape shape;    //background shape
	std::vector<Brick> bricks;
	int bricks_show[18];
	sf::RenderWindow window;

	// Define the paddles properties
	const sf::Time AITime = sf::seconds (0.1f);	
	sf::Clock clock;
	sf::Event event;

	enum states { INTRO, MODE1, MODE2, RESUME1, RESUME2, P1WIN, P1LOST };
	int gameState = INTRO;
	int isSlow = 0;
	int life = 3;
	int score = 0;
	float deltaTime;
	int countBrick;
	float fringe;
	int sector;	

	Gameplay () : window(sf::VideoMode (gameWidth, gameHeight, 32), "Breakout++") {

	}
	int init ();
	void restart ();
	void updateScore ();
	void updateLife ();
	int selectMode (sf::RenderWindow& window);
	int gameMode1 ();
	void gameMode2 ();
	void level1 ();
	void level2 ();
	void renderFrame (); // Draw game objects
	int isWin ();
};