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
	
	sf::Text won ("You won!\nPress space to restart or\nesc to exit.", fontSansation, 40);
	won.setPosition (gameWidth / 2 - won.getGlobalBounds ().width / 2, gameHeight / 2 - won.getGlobalBounds ().height / 2);
	won.setFillColor (sf::Color::White);

	sf::Text lost ("You lost!\nPress space to restart or\nesc to exit.", fontSansation, 40);
	lost.setPosition (gameWidth / 2 - lost.getGlobalBounds ().width / 2, gameHeight / 2 - lost.getGlobalBounds ().height / 2);
	lost.setFillColor (sf::Color::White);

	score.setFont (fontArial);
	score.setCharacterSize (30);
	score.setFillColor (sf::Color (239, 187, 56));

	// welcome background	
	bgTex.loadFromFile ("resources/snow.jpg");
	sf::Vector2f sz ((float)window.getSize ().x, (float)window.getSize ().y);
	shape.setSize (sz);
	shape.setTexture (&bgTex);

	// select game mode
	gameState = selectMode (window);

	//Game loop window
	while (window.isOpen ()) {
		// Handle events, esc to escape
		while (window.pollEvent (event)) {	
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				window.close ();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) {
				if (gameState != MODE1 || MODE2) {
					gameState = selectMode (window);
					p1Score = 0;
					p2Score = 0;
					updateScore ();
					restart ();
				}
			}
		}
		// mode 1, single player
		if (gameState == MODE1) {
			gameMode1 ();

			window.clear ();
			window.draw (shape);
			window.draw (leftPaddle.paddle);
			window.draw (rightPaddle.paddle);
			window.draw (score);
			window.draw (pong.ball);
		}

		else if (gameState == MODE2) {
			window.clear ();
			window.draw (shape);
		}
		
		else if (gameState == P1WON) {
			window.clear ();
			window.draw (shape);
			window.draw (won);
		}

		else if (gameState == P1LOST) {
			window.clear (); 
			window.draw (shape);
			window.draw (lost);
		}

		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::restart () {
	gameState = MODE1;
	clock.restart ();

	// Reset the position of the paddles and ball
	leftPaddle.paddle.setPosition (10 + leftPaddle.paddleSize.x / 2, gameHeight / 2);
	rightPaddle.paddle.setPosition (gameWidth - 10 - rightPaddle.paddleSize.x / 2, gameHeight / 2);

	pong.ball.setPosition (gameWidth / 2, gameHeight / 2);
	pong.ballSpeed = 400.f;

	// Reset the ball angle
	do {
		// Make sure the ball initial angle is not too much vertical
		pong.ballAngle = (std::rand () % 360) * 2 * pi / 360;
	} while (std::abs (std::cos (pong.ballAngle)) < 0.7f);
}

void Gameplay::updateScore () {
	std::stringstream str;
	str << p1Score << "   " << p2Score;
	score.setString (str.str ());
	score.setPosition (10, 10);
}

int Gameplay::selectMode (sf::RenderWindow& window) {
	// welcome text
	sf::Text welcome[3];
	std::string msg[3] = { "Press 1 - You VS Ai; 2 - You + Friend VS 2 Ai" , "Player1 uses WSAD, Player2 uses Arrow Keys.", "Press Esc to exit." };
	float y[3] = { 50.0f, 100.0f, 150.0f };
	for (int i = 0; i < 3; i++) {
		welcome[i].setPosition (50.0f, y[i]);
		welcome[i].setFillColor (sf::Color (239, 187, 56));
		welcome[i].setCharacterSize (30);
		welcome[i].setString (msg[i]);
		welcome[i].setFont (fontSansation);
	}	

	// main loop
	while (true) {
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close ();
				return 0;
			}
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num1) {
				gameState = MODE1;
				window.clear ();
				return 1;
			} else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num2) {
				gameState = MODE2;
				window.clear ();
				return 2;
			}
		}
		window.clear ();
		// put text and wait for select
		window.draw (shape);
		for (int i = 0; i < 3; i++)
			window.draw (welcome[i]);

		window.display ();
	}
	return 1;
}

void Gameplay::gameMode1 () {
	float deltaTime = clock.restart ().asSeconds ();

	// Move the player's paddle
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up) &&
		(leftPaddle.paddle.getPosition ().y - leftPaddle.paddleSize.y / 2 > 5.f)) {
		leftPaddle.paddle.move (0.f, -leftPaddle.paddleSpeed * deltaTime);
	}
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down) &&
		(leftPaddle.paddle.getPosition ().y + leftPaddle.paddleSize.y / 2 < gameHeight - 5.f)) {
		leftPaddle.paddle.move (0.f, leftPaddle.paddleSpeed * deltaTime);
	}

	// Move the computer's paddle
	if (((rightPaddle.rightPaddleSpeed < 0.f) && (rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2 > 5.f)) ||
		((rightPaddle.rightPaddleSpeed > 0.f) && (rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2 < gameHeight - 5.f))) {
		rightPaddle.paddle.move (0.f, rightPaddle.rightPaddleSpeed * deltaTime);
	}

	// Update the computer's paddle direction according to the ball position
	if (AITimer.getElapsedTime () > AITime) {
		AITimer.restart ();
		if (pong.ball.getPosition ().y + pong.ballRadius > rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2)
			rightPaddle.rightPaddleSpeed = rightPaddle.paddleSpeed;
		else if (pong.ball.getPosition ().y - pong.ballRadius < rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2)
			rightPaddle.rightPaddleSpeed = -rightPaddle.paddleSpeed;
		else
			rightPaddle.rightPaddleSpeed = 0.f;
	}

	// Move the ball
	float factor = pong.ballSpeed * deltaTime;
	pong.ball.move (std::cos (pong.ballAngle) * factor, std::sin (pong.ballAngle) * factor);

	// Check collisions between the ball and the screen
	if (pong.ball.getPosition ().x - pong.ballRadius < 0.f) {
		p2Score++;
		updateScore ();

		restart ();
	}
	if (pong.ball.getPosition ().x + pong.ballRadius > gameWidth) {
		p1Score++;
		updateScore ();

		restart ();
	}
	if (pong.ball.getPosition ().y - pong.ballRadius < 0.f) {
		pong.ballSound.play ();
		pong.ballAngle = -pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, pong.ballRadius + 0.1f);
	}
	if (pong.ball.getPosition ().y + pong.ballRadius > gameHeight) {
		pong.ballSound.play ();
		pong.ballAngle = -pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, gameHeight - pong.ballRadius - 0.1f);
	}

	// Check the collisions between the ball and the paddles
	// Left Paddle
	if (pong.ball.getPosition ().x - pong.ballRadius < leftPaddle.paddle.getPosition ().x + leftPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x - pong.ballRadius > leftPaddle.paddle.getPosition ().x &&
		pong.ball.getPosition ().y + pong.ballRadius >= leftPaddle.paddle.getPosition ().y - leftPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y - pong.ballRadius <= leftPaddle.paddle.getPosition ().y + leftPaddle.paddleSize.y / 2) {
		//Accelerate the ball speed after each collision with the left paddle, increasing by 25 each time
		pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

		if (pong.ball.getPosition ().y > leftPaddle.paddle.getPosition ().y)
			pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
		else
			pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

		pong.ballSound.play ();
		pong.ball.setPosition (leftPaddle.paddle.getPosition ().x + pong.ballRadius + leftPaddle.paddleSize.x / 2 + 0.1f, pong.ball.getPosition ().y);
	}

	// Right Paddle
	if (pong.ball.getPosition ().x + pong.ballRadius > rightPaddle.paddle.getPosition ().x - rightPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x + pong.ballRadius < rightPaddle.paddle.getPosition ().x &&
		pong.ball.getPosition ().y + pong.ballRadius >= rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y - pong.ballRadius <= rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2) {
		//Accelerate the ball speed after each collision with the right paddle, increasing by 25 each time
		pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

		if (pong.ball.getPosition ().y > rightPaddle.paddle.getPosition ().y)
			pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
		else
			pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

		pong.ballSound.play ();
		pong.ball.setPosition (rightPaddle.paddle.getPosition ().x - pong.ballRadius - rightPaddle.paddleSize.x / 2 - 0.1f, pong.ball.getPosition ().y);
	}

	// detect if game is over
	if (p1Score >= 5) {
		gameState = P1WON;
	}
	if (p2Score >= 5) {
		gameState = P1LOST;
	}
}