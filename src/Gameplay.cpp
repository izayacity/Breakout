#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include "../include/Gameplay.h"
#include "../include/Collision.h"
#include <sstream>
#include <iostream>

int Gameplay::init () {	
	std::srand (static_cast<unsigned int>(std::time (NULL)));

	// Create the window of the application
	sf::RenderWindow window (sf::VideoMode (gameWidth, gameHeight, 32), "Breakout++");
	window.setFramerateLimit (60); // call it once, after creating the window

	// Load the text font
	if (!fontSansation.loadFromFile ("resources/sansation.ttf"))
		return EXIT_FAILURE;
	if (!fontArial.loadFromFile ("resources/arial.ttf"))
		return EXIT_FAILURE;
	
	// Win text
	sf::Text won ("You won!\nPress space to restart or\nesc to exit.", fontSansation, 40);
	won.setPosition (gameWidth / 2 - won.getGlobalBounds ().width / 2, gameHeight / 2 - won.getGlobalBounds ().height / 2);
	won.setFillColor (sf::Color::White);

	// Lost text
	sf::Text lost ("You lost!\nPress space to restart or\nesc to exit.", fontSansation, 40);
	lost.setPosition (gameWidth / 2 - lost.getGlobalBounds ().width / 2, gameHeight / 2 - lost.getGlobalBounds ().height / 2);
	lost.setFillColor (sf::Color::White);

	// Score text
	score.setFont (fontArial);
	score.setCharacterSize (30);
	score.setFillColor (sf::Color (239, 187, 56));

	// Welcome background	
	bgTex.loadFromFile ("resources/snow.jpg");
	sf::Vector2f sz ((float)window.getSize ().x, (float)window.getSize ().y);
	shape.setSize (sz);
	shape.setTexture (&bgTex);

	// Select game mode
	gameState = selectMode (window);

	// Game loop window
	while (window.isOpen ()) {
		// Handle events, esc to escape
		while (window.pollEvent (event)) {
			// Render the start window
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				// Select game mode
				gameState = selectMode (window);  //To be made as resumeWindow ()

				if (gameState == -1)
					return EXIT_SUCCESS;
				break;
			}
		}

		// Mode 1, single player
		if (gameState == MODE1 || gameState == RESUME1) {
			gameMode1 ();

			window.clear ();
			window.draw (shape);
			window.draw (myPaddle.paddle);
			window.draw (score);
			window.draw (pong.ball);
		}

		// Mode 1, single player
		if (gameState == MODE2 || gameState == RESUME2) {

		}

		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::restart () {
	gameState = MODE1;
	clock.restart ();

	// Reset the position of the paddles and ball
	myPaddle.paddle.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y / 2);

	pong.ball.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y - pong.ballRadius - 10);
	pong.ballSpeed = 400.f;
}

void Gameplay::updateScore () {
	std::stringstream str;
	str << p1Score << "   " << p2Score;
	score.setString (str.str ());
	score.setPosition (10, 10);
}

int Gameplay::selectMode (sf::RenderWindow& window) {
	// Welcome text
	sf::Text welcome[5];
	std::string msg[5] = { "Press 1 - You VS Ai", "2 - You + Friend VS 2 Ai" , "Player1 uses WSAD", "Player2 uses Arrow Keys.", "Press Esc to exit." };
	for (int i = 0; i < 5; i++) {
		welcome[i].setPosition (50.0f, 50.0f * (i + 1));
		welcome[i].setFillColor (sf::Color (239, 187, 56));
		welcome[i].setCharacterSize (30);
		welcome[i].setString (msg[i]);
		welcome[i].setFont (fontSansation);
	}	

	// Main loop
	while (true) {
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close ();
				return -1;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num1) {
				gameState = MODE1;
				p1Score = 0;
				p2Score = 0;
				updateScore ();
				restart ();
				window.clear ();
				return gameState;
			} else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num2) {
				gameState = MODE2;
				p1Score = 0;
				p2Score = 0;
				updateScore ();
				restart ();
				window.clear ();
				return gameState;
			}
		}
		window.clear ();
		// Put text and wait for select
		window.draw (shape);
		for (int i = 0; i < 5; i++)
			window.draw (welcome[i]);

		window.display ();
	}
}

void Gameplay::gameMode1 () {
	float deltaTime = clock.restart ().asSeconds ();

	// Move the player's paddle
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up) &&
		(myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 > 5.f)) {
		myPaddle.paddle.move (0.f, -myPaddle.paddleSpeed * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down) &&
		(myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2 < gameHeight - 5.f)) {
		myPaddle.paddle.move (0.f, myPaddle.paddleSpeed * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left) &&
		(myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 > 5.f)) {
		myPaddle.paddle.move (-myPaddle.paddleSpeed * deltaTime, 0.f);
	}
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right) &&
		(myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 < gameWidth - 5.f)) {
		myPaddle.paddle.move (myPaddle.paddleSpeed * deltaTime, 0.f);
	}

}