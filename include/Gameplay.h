#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include "../include/Ball.h"
#include "../include/Paddle.h"
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
	Paddle Paddle2;

	sf::Font fontHNMed;
	sf::Font fontHNM;
	sf::Font fontHNL;
	sf::Text pauseMessage;
	sf::Clock AITimer;
	sf::Text scoreText;
	sf::Text lifeText;
	sf::Texture bgTex;    //background texture
	sf::Texture hard_brick_tex;
	sf::Texture normal_brick_tex;
	sf::RectangleShape shape;    //background shape
	std::vector<Brick> bricks;
	int bricks_show[60];
	sf::RenderWindow window;

	sf::SoundBuffer paddle_sound_buffer;
	sf::Sound paddle_sound;
	sf::SoundBuffer destroy_sound_buffer;
	sf::Sound destroy_sound;
	sf::SoundBuffer damage_sound_buffer;
	sf::Sound damage_sound;
	sf::SoundBuffer wall_sound_buffer;
	sf::Sound wall_sound;
	sf::SoundBuffer win_sound_buffer;
	sf::Sound win_sound;
	sf::SoundBuffer lose_sound_buffer;
	sf::Sound lose_sound;

	// Define the paddles properties
	const sf::Time AITime = sf::seconds (0.1f);	
	sf::Clock clock;
	sf::Event event;

	const int BRICK_COUNT;
	enum states { INTRO, MODE1, MODE2, MODE10, MODE20, M1L1, M1L2, M2L1, M2L2, RESUME2, LOST };
	int gameState = INTRO;
	int isSlow = 0;
	int life = 3;
	int score = 0;
	float deltaTime;
	int countBrick;
	float fringe;
	int sector;

	Gameplay () : window(sf::VideoMode (gameWidth, gameHeight, 32), "Breakout++"), BRICK_COUNT(60) {

	}
	int init ();
	void restart ();
	void updateScore ();
	void updateLife ();
	int selectMode (sf::RenderWindow& window);
	void update_state ();
	void level0 ();
	void level1 ();
	void level2 ();
	void renderFrame (); // Draw game objects
	int isWin ();
	void collisionResult (int index);
};