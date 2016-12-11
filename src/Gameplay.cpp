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
#include <vector>
#include <assert.h>

#define BRICKS1 18

void Gameplay::restart () {	
	gameState = MODE1;
	clock.restart ();

	// Reset the position of the paddles and ball
	myPaddle.paddle.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y / 2);
	pong.ball.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y - pong.ballRadius - 3);
	pong.ballSpeed = 450.f;

	// Reset the ball angle
	do {
		// Make sure the ball initial angle is not too much vertical
		pong.ballAngle = -(std::rand () % 180) * pi / 180;
	} while (std::abs (std::cos (pong.ballAngle)) <= 0.5f || std::abs (std::cos (pong.ballAngle)) >= 0.87);
}

int Gameplay::init () {	
	std::srand (static_cast<unsigned int>(std::time (NULL)));
	window.setFramerateLimit (60); // call it once, after creating the window

	// Load the text font
	if (!fontHNMed.loadFromFile ("resources/HelveticaNeueMed.ttf"))
		return EXIT_FAILURE;
	if (!fontHNM.loadFromFile ("resources/HelveticaNeue Medium.ttf"))
		return EXIT_FAILURE;
	if (!fontHNL.loadFromFile ("resources/HelveticaNeue Light.ttf"))
		return EXIT_FAILURE;	

	// Life text
	lifeText.setFont (fontHNL);
	lifeText.setCharacterSize (30);
	lifeText.setFillColor (sf::Color::Red);
	lifeText.setString ("Life: 3");
	lifeText.setPosition (10, gameHeight - 40);

	// Score text
	score.setFont (fontHNL);
	score.setCharacterSize (30);
	score.setFillColor (sf::Color (239, 187, 56));

	// Welcome background	
	bgTex.loadFromFile ("resources/snow.jpg");
	sf::Vector2f sz ((float)window.getSize ().x, (float)window.getSize ().y);
	shape.setSize (sz);
	shape.setTexture (&bgTex);

	// Select game mode
	gameState = selectMode (window);
	restart ();

	// Game loop window
	while (window.isOpen ()) {
		while (window.pollEvent (event)) {
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				gameState = selectMode (window);  //To be made as resumeWindow ()
				restart ();
				if (gameState == -1)
					return EXIT_SUCCESS;
				break;
			}
			if (gameState == MODE1 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = RESUME1;
			}
		}

		deltaTime = clock.restart ().asSeconds ();

		if (gameState == RESUME1) {
			gameMode1 ();
		}
		
		renderFrame ();
		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::renderFrame () {
	// Win text
	sf::Text wonText ("You won!\nPress esc to menu.", fontHNM, 40);
	wonText.setPosition (gameWidth / 2 - wonText.getGlobalBounds ().width / 2, gameHeight / 2 - wonText.getGlobalBounds ().height / 2);
	wonText.setFillColor (sf::Color::White);

	// Lost text
	sf::Text lostText ("You lost!\nPress esc to menu.", fontHNM, 40);
	lostText.setPosition (gameWidth / 2 - lostText.getGlobalBounds ().width / 2, gameHeight / 2 - lostText.getGlobalBounds ().height / 2);
	lostText.setFillColor (sf::Color::White);

	window.clear ();
	window.draw (shape);
	window.draw (myPaddle.paddle);
	window.draw (pong.ball);
	window.draw (lifeText);

	if (gameState == MODE1 || gameState == RESUME1) {
		int i = 0;
		for (std::vector<Brick>::iterator it = bricks.begin (); it != bricks.end (); ++it) {
			if (bricks_show[i] == 1) {
				window.draw ((*it).brick);
			}
			i++;
		}
	} else if (gameState == P1LOST) {
		window.draw (lostText);
	} else if (gameState == P1WIN) {
		window.draw (wonText);
	}
}

void Gameplay::updateScore () {
	std::stringstream str;
	str << p1Score << "   " << p2Score;
	score.setString (str.str ());
	score.setPosition (10, 10);
}

void Gameplay::updateLife () {
	std::stringstream str;
	str << "Life: " << life;
	lifeText.setString (str.str ());
	lifeText.setPosition (10, gameHeight - 40);
}

int Gameplay::selectMode (sf::RenderWindow& window) {
	life = 3;
	updateLife ();
	for (int i = 0; i < 18; i++) {
		bricks_show[i] = 1;
	}
	// Welcome text
	sf::Text welcome[5];
	std::string msg[5] = { "Press 1 - You VS Ai", "2 - You + Friend VS 2 Ai" , "Player1 uses WSAD", "Player2 uses Arrow Keys.", "Press Esc to exit." };
	for (int i = 0; i < 5; i++) {
		welcome[i].setPosition (50.0f, 50.0f * (i + 1));
		welcome[i].setFillColor (sf::Color (239, 187, 56));
		welcome[i].setCharacterSize (30);
		welcome[i].setString (msg[i]);
		welcome[i].setFont (fontHNMed);
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
				level1 ();
				window.clear ();
				return gameState;
			} else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num2) {
				gameState = MODE2;
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

void Gameplay::level1 () {
	for (int i = 0; i < 6; i++) {
		bricks.push_back (Brick ());
		std::rand () % 2 == 0 ? bricks[i].init1 () : bricks[i].init2();
		bricks[i].brick.setPosition (i * 100 + 50.f, 12.5f);
	}

	for (int i = 6; i < 12; i++) {
		bricks.push_back (Brick ());
		std::rand () % 2 == 0 ? bricks[i].init1 () : bricks[i].init2 ();
		bricks[i].brick.setPosition ((i - 6) * 100 + 50.f, 37.5f);
	}

	for (int i = 12; i < 18; i++) {
		bricks.push_back (Brick ());
		std::rand () % 2 == 0 ? bricks[i].init1 () : bricks[i].init2 ();
		bricks[i].brick.setPosition ((i - 12) * 100 + 50.f, 62.5f);
	}
}

int Gameplay::gameMode1 () {	
	// Move the player's paddle
	//if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up) &&
	//	(myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 > gameHeight / 2)) {
	//	myPaddle.paddle.move (0.f, -myPaddle.paddleSpeed * deltaTime);
	//}

	//if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down) &&
	//	(myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2 < gameHeight - 5.f)) {
	//	myPaddle.paddle.move (0.f, myPaddle.paddleSpeed * deltaTime);
	//}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left) &&
		(myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 > 5.f)) {
		myPaddle.paddle.move (-myPaddle.paddleSpeed * deltaTime, 0.f);
	}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right) &&
		(myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 < gameWidth - 5.f)) {
		myPaddle.paddle.move (myPaddle.paddleSpeed * deltaTime, 0.f);
	}

	// Move the ball
	float factor = pong.ballSpeed * deltaTime;	
	pong.ball.move (std::cos (pong.ballAngle) * factor, std::sin (pong.ballAngle) * factor);

	// Check the collisions between the ball and the paddles
	if (pong.ball.getPosition ().y + pong.ballRadius > myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y + pong.ballRadius < myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().x >= myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x <= myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2) {		
		pong.ballAngle = -pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, myPaddle.paddle.getPosition ().y - pong.ballRadius - myPaddle.paddleSize.y / 2 - 0.1f);
		pong.ballSound.play ();
	}

	// Check collisions between the ball and the screen
	else if (pong.ball.getPosition ().y - pong.ballRadius < 0.f) {
		pong.ballSound.play ();
		pong.ballAngle = - pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, pong.ballRadius + 0.1f);
	}

	else if (pong.ball.getPosition ().y + pong.ballRadius > gameHeight) {
		pong.ballAngle = - pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, gameHeight - pong.ballRadius - 0.1f);

		life--;	
		updateLife ();

		if (life > 0) {
			gameState = MODE1;
			restart ();
		} else {
			gameState = P1LOST;
		}
				
		return gameState;
	}

	else if (pong.ball.getPosition ().x - pong.ballRadius < 0.f) {
		pong.ballSound.play ();
		pong.ballAngle = - (pi + pong.ballAngle);
		pong.ball.setPosition (pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
	}

	else if (pong.ball.getPosition ().x + pong.ballRadius > gameWidth) {
		pong.ballSound.play ();
		pong.ballAngle = (pi - pong.ballAngle);
		pong.ball.setPosition (gameWidth - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
	}

	// Check the collisions between the ball and the bricks
	else {
		if (pong.ball.getPosition ().y - pong.ballRadius <= 26.f) {
			sector = (int)(pong.ball.getPosition ().x / 100);

			if (bricks_show[sector] == 1 &&
				pong.ball.getPosition ().x >= bricks[sector].brick.getPosition ().x - 100.f / 2 &&
				pong.ball.getPosition ().x <= bricks[sector].brick.getPosition ().x + 100.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius > bricks[sector].brick.getPosition ().y - 25.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius < bricks[sector].brick.getPosition ().y + 25.f / 2) {

				if (bricks[sector].type == 1) {
					if (bricks[sector].life == 2) {
						assert (bricks[sector].brickTex.loadFromFile ("resources/broke.jpg"));
						bricks[sector].brickTex.setSmooth (true);
						bricks[sector].brick.setTexture (&bricks[sector].brickTex);
						bricks[sector].life--;
					} else if (bricks[sector].life <= 1) {
						bricks_show[sector] = 0;
					}
				} else {
					bricks_show[sector] = 0;
				}

				pong.ballSound.play ();
				pong.ballAngle = -pong.ballAngle;
				pong.ball.setPosition (pong.ball.getPosition ().x, 25.f + pong.ballRadius + 0.1f);
				
				if (isWin ()) {
					gameState = P1WIN;
					return gameState;
				}
			}
		}

		else if (pong.ball.getPosition ().y - pong.ballRadius <= 51.f && pong.ball.getPosition ().y - pong.ballRadius >= 25.f) {
			sector = (int)(pong.ball.getPosition ().x / 100);

			if (bricks_show[6 + sector] == 1 &&
				pong.ball.getPosition ().x >= bricks[6 + sector].brick.getPosition ().x - 100.f / 2 &&
				pong.ball.getPosition ().x <= bricks[6 + sector].brick.getPosition ().x + 100.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius > bricks[6 + sector].brick.getPosition ().y - 25.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius < bricks[6 + sector].brick.getPosition ().y + 25.f / 2) {

				if (bricks[6 + sector].type == 1) {
					if (bricks[6 + sector].life == 2) {
						assert (bricks[6 + sector].brickTex.loadFromFile ("resources/broke.jpg"));
						bricks[6 + sector].brickTex.setSmooth (true);
						bricks[6 + sector].brick.setTexture (&bricks[6 + sector].brickTex);
						bricks[6 + sector].life--;
					} else if (bricks[6 + sector].life <= 1) {
						bricks_show[6 + sector] = 0;
					}
				} else {
					bricks_show[6 + sector] = 0;
				}

				pong.ballSound.play ();
				pong.ballAngle = -pong.ballAngle;
				pong.ball.setPosition (pong.ball.getPosition ().x, 50.f + pong.ballRadius + 0.1f);

				if (isWin ()) {
					gameState = P1WIN;
					return gameState;
				}
			}
		}

		else if (pong.ball.getPosition ().y - pong.ballRadius <= 76.f && pong.ball.getPosition ().y - pong.ballRadius >= 50.f) {
			sector = (int)(pong.ball.getPosition ().x / 100);

			if (bricks_show[12 + sector] == 1 &&
				pong.ball.getPosition ().x >= bricks[12 + sector].brick.getPosition ().x - 100.f / 2 &&
				pong.ball.getPosition ().x <= bricks[12 + sector].brick.getPosition ().x + 100.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius > bricks[12 + sector].brick.getPosition ().y - 25.f / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius < bricks[12 + sector].brick.getPosition ().y + 25.f / 2) {

				if (bricks[12 + sector].type == 1) {
					if (bricks[12 + sector].life == 2) {
						assert (bricks[12 + sector].brickTex.loadFromFile ("resources/broke.jpg"));
						bricks[12 + sector].brickTex.setSmooth (true);
						bricks[12 + sector].brick.setTexture (&bricks[12 + sector].brickTex);
						bricks[12 + sector].life--;
					} else if (bricks[12 + sector].life <= 1) {
						bricks_show[12 + sector] = 0;
					}
				} else {
					bricks_show[12 + sector] = 0;
				}

				pong.ballSound.play ();
				pong.ballAngle = -pong.ballAngle;
				pong.ball.setPosition (pong.ball.getPosition ().x, 75.f + pong.ballRadius + 0.1f);

				if (isWin ()) {
					gameState = P1WIN;
					return gameState;
				}
			}
		}
	}

	return gameState;
}

int Gameplay::isWin () {
	for (auto i : bricks_show) {
		if (i == 1)
			return 0;
	}
	return 1;
}