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

void Gameplay::restart () {	
	clock.restart ();

	// Reset the position of the paddles and ball
	if (gameState == MODE1 || gameState == MODE10 || gameState == M1L1 || gameState == M1L2) {
		myPaddle.paddle.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y / 2);
	} else if (gameState == MODE2 || gameState == MODE20 || gameState == M2L1 || gameState == M2L2) {
		myPaddle.paddle.setPosition (gameWidth / 3, gameHeight - myPaddle.paddleSize.y / 2);
		Paddle2.paddle.setPosition (gameWidth * 2 / 3, gameHeight - myPaddle.paddleSize.y / 2);
	}

	pong.ball.setPosition (gameWidth / 2, gameHeight - myPaddle.paddleSize.y - pong.ballRadius - 3);

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
	scoreText.setFont (fontHNM);
	scoreText.setCharacterSize (30);
	scoreText.setFillColor (sf::Color (239, 187, 56));
	scoreText.setString ("Score: 0");
	scoreText.setPosition (10, gameHeight - 75);

	// Sound effects
	assert (paddle_sound_buffer.loadFromFile ("resources/paddle.wav"));
	paddle_sound.setBuffer (paddle_sound_buffer);
	assert (destroy_sound_buffer.loadFromFile ("resources/destroy.wav"));
	destroy_sound.setBuffer (destroy_sound_buffer);
	assert (damage_sound_buffer.loadFromFile ("resources/damage.wav"));
	damage_sound.setBuffer (damage_sound_buffer);
	assert (wall_sound_buffer.loadFromFile ("resources/wall.wav"));
	wall_sound.setBuffer (wall_sound_buffer);
	assert (lose_sound_buffer.loadFromFile ("resources/lose.wav"));
	lose_sound.setBuffer (lose_sound_buffer);
	assert (win_sound_buffer.loadFromFile ("resources/win.wav"));
	win_sound.setBuffer (win_sound_buffer);

	// Welcome background	
	bgTex.loadFromFile ("resources/snow.jpg");
	sf::Vector2f sz ((float)window.getSize ().x, (float)window.getSize ().y);
	shape.setSize (sz);
	shape.setTexture (&bgTex);

	// Select game mode, start from level1
	gameState = selectMode (window);
	level0 ();
	level1 ();
	restart ();

	// Game loop window
	while (window.isOpen ()) {
		while (window.pollEvent (event)) {
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				gameState = selectMode (window);  //To be made as resumeWindow ()
				level0 ();
				level1 ();
				restart ();
				if (gameState == -1)
					return EXIT_SUCCESS;
				break;
			}
			// Enter level1 in single player mode1 when pressing space key
			else if (gameState == MODE1 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = M1L1;
			}
			// Enter level2 in single player mode1 when pressing space key
			else if (gameState == MODE10 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = M1L2;
			}
			// Enter level1 in single player mode2 when pressing space key
			else if (gameState == MODE2 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = M2L1;
			}
			// Enter level2 in single player mode2 when pressing space key
			else if (gameState == MODE20 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				gameState = M2L2;
			}
		}

		deltaTime = clock.restart ().asSeconds ();

		if (gameState == M1L1 || gameState == M1L2 || gameState == M2L1 || gameState == M2L2) {
			update_state ();
		}

		renderFrame ();
		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::renderFrame () {
	sf::Text lostText ("You lost!\nPress esc to menu.", fontHNM, 40);
	lostText.setPosition (gameWidth / 2 - lostText.getGlobalBounds ().width / 2, gameHeight / 2 - lostText.getGlobalBounds ().height / 2);
	lostText.setFillColor (sf::Color::White);

	window.clear ();
	window.draw (shape);
	window.draw (myPaddle.paddle);
	window.draw (pong.ball);
	window.draw (lifeText);
	window.draw (scoreText);

	if (gameState == MODE2 || gameState == MODE20 || gameState == M2L1 || gameState == M2L2) {
		window.draw (Paddle2.paddle);
	}

	if (gameState == MODE1 || gameState == MODE10 || gameState == M1L1 || gameState == M1L2 || gameState == MODE2 || gameState == MODE20 || gameState == M2L1 || gameState == M2L2) {
		int i = 0;
		for (std::vector<Brick>::iterator it = bricks.begin (); it != bricks.end (); ++it) {
			if (bricks_show[i] == 1) {				
				window.draw ((*it).brick);
			}
			i++;
		}
	} else if (gameState == LOST) {
		window.draw (lostText);
	}
}

void Gameplay::updateScore () {
	std::stringstream str;
	str << "Score: " << score;
	scoreText.setString (str.str ());
	scoreText.setPosition (10, gameHeight - 75);
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
	score = 0;
	updateScore ();

	// Welcome text
	sf::Text welcome[5];
	std::string msg[5] = { "Press 1 - Single player(keyboard)", "2 - Double Player" , "Player1 uses WSAD", "Player2 uses Arrow Keys.", "Press Esc to exit." };
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
			// Escape key to close the window
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close ();
				return -1;
			}
			// Num1 key to enter mode1 single player
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num1) {
				gameState = MODE1;				
				window.clear ();
				return gameState;
			}
			// Num2 key to enter mode2 double player2
			else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Num2) {
				gameState = MODE2;
				Paddle2.paddle.setFillColor (sf::Color (109, 192, 102));
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

void Gameplay::level0 () {
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 6; i++) {
			bricks.push_back (Brick ());
			if(j != 0)
				std::rand () % 2 == 0 ? bricks[i + 6 * j].init1 () : bricks[i + 6 * j].init2 ();
			bricks[i + 6 * j].brick.setPosition (i * 100 + 50.f, 12.5f + 25.f * j);
		}
	}
	for (int i = 0; i < 6; i++) {
		bricks[i].init3 ();
	}
}

void Gameplay::level1 () {
	for (int i = 0; i < 6; i++) {
		bricks_show[i] = 0;
	}

	for (int i = 6; i < BRICK_COUNT; i++) {
		bricks_show[i] = 1;
	}

	for (int j = 1; j < 9; j++) {
		for (int i = 0; i < 6; i++) {
			if(j == 1 || j == 2 || j == 4 || j == 5 || j == 7 || j == 8)
				bricks_show[j * 6 + i] = 0;
		}
	}

	for (int j = 1; j < 10; j++) {
		bricks_show[j * 6 + 1] = 0;
		bricks_show[j * 6 + 4] = 0;
	}	
}

void Gameplay::level2 () {
	for (int i = 0; i < 6; i++) {
		bricks_show[i] = 0;
	}

	for (int i = 6; i < BRICK_COUNT; i++) {
		bricks_show[i] = 1;
	}
	
	for (int i = 6; i < 59; i = i + 2) {
		bricks_show[i] = 0;
	}

	for (int i = 0; i < 6; i++) {
		bricks[i].init3 ();
	}
}

void Gameplay::update_state () {
	// Move the ball
	float factor = pong.ballSpeed * deltaTime;
	pong.ball.move (std::cos (pong.ballAngle) * factor, std::sin (pong.ballAngle) * factor);

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left) &&
		(myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 > 5.f)) {
		myPaddle.paddle.move (-myPaddle.paddleSpeed * deltaTime, 0.f);
	}

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right) &&
		(myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 < gameWidth - 5.f)) {
		myPaddle.paddle.move (myPaddle.paddleSpeed * deltaTime, 0.f);
	}

	if (gameState == M2L1 || gameState == M2L2) {
		if (sf::Keyboard::isKeyPressed (sf::Keyboard::A) &&
			(Paddle2.paddle.getPosition ().x - Paddle2.paddleSize.x / 2 > 5.f)) {
			Paddle2.paddle.move (-Paddle2.paddleSpeed * deltaTime, 0.f);
		}

		if (sf::Keyboard::isKeyPressed (sf::Keyboard::D) &&
			(Paddle2.paddle.getPosition ().x + Paddle2.paddleSize.x / 2 < gameWidth - 5.f)) {
			Paddle2.paddle.move (Paddle2.paddleSpeed * deltaTime, 0.f);
		}

		// Check the collisions between the ball and the paddles
		if (pong.ball.getPosition ().y + pong.ballRadius > Paddle2.paddle.getPosition ().y - Paddle2.paddleSize.y / 2 &&
			pong.ball.getPosition ().y + pong.ballRadius < Paddle2.paddle.getPosition ().y + Paddle2.paddleSize.y / 2 &&
			pong.ball.getPosition ().x >= Paddle2.paddle.getPosition ().x - Paddle2.paddleSize.x / 2 &&
			pong.ball.getPosition ().x <= Paddle2.paddle.getPosition ().x + Paddle2.paddleSize.x / 2) {
			pong.ballAngle = -pong.ballAngle;
			pong.ball.setPosition (pong.ball.getPosition ().x, Paddle2.paddle.getPosition ().y - pong.ballRadius - Paddle2.paddleSize.y / 2 - 0.1f);
			paddle_sound.play ();
		} else if (pong.ball.getPosition ().x + pong.ballRadius > Paddle2.paddle.getPosition ().x - Paddle2.paddleSize.x / 2 &&
			pong.ball.getPosition ().x + pong.ballRadius < Paddle2.paddle.getPosition ().x + Paddle2.paddleSize.x / 2 &&
			pong.ball.getPosition ().y >= Paddle2.paddle.getPosition ().y - Paddle2.paddleSize.y / 2 &&
			pong.ball.getPosition ().y <= Paddle2.paddle.getPosition ().y + Paddle2.paddleSize.y / 2) {
			pong.ballAngle = (pi - pong.ballAngle);
			pong.ball.setPosition (Paddle2.paddle.getPosition ().x - Paddle2.paddleSize.x / 2 - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
			paddle_sound.play ();
		} else if (pong.ball.getPosition ().x - pong.ballRadius > Paddle2.paddle.getPosition ().x - Paddle2.paddleSize.x / 2 &&
			pong.ball.getPosition ().x - pong.ballRadius < Paddle2.paddle.getPosition ().x + Paddle2.paddleSize.x / 2 &&
			pong.ball.getPosition ().y >= Paddle2.paddle.getPosition ().y - Paddle2.paddleSize.y / 2 &&
			pong.ball.getPosition ().y <= Paddle2.paddle.getPosition ().y + Paddle2.paddleSize.y / 2) {
			pong.ballAngle = -(pi + pong.ballAngle);
			pong.ball.setPosition (Paddle2.paddle.getPosition ().x + Paddle2.paddleSize.x / 2 + pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
			paddle_sound.play ();
		}
	}

	// Check the collisions between the ball and the paddles
	if (pong.ball.getPosition ().y + pong.ballRadius > myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y + pong.ballRadius < myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().x >= myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x <= myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2) {
		pong.ballAngle = -pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, myPaddle.paddle.getPosition ().y - pong.ballRadius - myPaddle.paddleSize.y / 2 - 0.1f);
		paddle_sound.play ();
	} else if (pong.ball.getPosition ().x + pong.ballRadius > myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x + pong.ballRadius < myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().y >= myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y <= myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2) {
		pong.ballAngle = (pi - pong.ballAngle);
		pong.ball.setPosition (myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
		paddle_sound.play ();
	} else if (pong.ball.getPosition ().x - pong.ballRadius > myPaddle.paddle.getPosition ().x - myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().x - pong.ballRadius < myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 &&
		pong.ball.getPosition ().y >= myPaddle.paddle.getPosition ().y - myPaddle.paddleSize.y / 2 &&
		pong.ball.getPosition ().y <= myPaddle.paddle.getPosition ().y + myPaddle.paddleSize.y / 2) {
		pong.ballAngle = -(pi + pong.ballAngle);
		pong.ball.setPosition (myPaddle.paddle.getPosition ().x + myPaddle.paddleSize.x / 2 + pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
		paddle_sound.play ();
	}

	// Check collisions between the ball and the screen
	else if (pong.ball.getPosition ().y - pong.ballRadius < 0.f) {
		wall_sound.play ();
		pong.ballAngle = - pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, pong.ballRadius + 0.1f);
	}

	else if (pong.ball.getPosition ().y + pong.ballRadius > gameHeight) {
		lose_sound.play ();
		pong.ballAngle = - pong.ballAngle;
		pong.ball.setPosition (pong.ball.getPosition ().x, gameHeight - pong.ballRadius - 0.1f);

		life--;	
		updateLife ();

		if (life > 0) {
			if (gameState == M1L1) {
				gameState = MODE1;
			} else if (gameState == M1L2) {
				gameState = MODE10;
			} else if (gameState == M2L1) {
				gameState = MODE2;
			} else if (gameState == M2L2) {
				gameState = MODE20;
			}
			restart ();
		} else {
			gameState = LOST;
		}
	}
	// Check collisions between the ball and the wall
	else if (pong.ball.getPosition ().x - pong.ballRadius < 0.f) {
		wall_sound.play ();
		pong.ballAngle = - (pi + pong.ballAngle);
		pong.ball.setPosition (pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
	}

	else if (pong.ball.getPosition ().x + pong.ballRadius > gameWidth) {
		wall_sound.play ();
		pong.ballAngle = (pi - pong.ballAngle);
		pong.ball.setPosition (gameWidth - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
	}

	// Check the collisions between the ball and the bricks, performance O(N)
	else {	
		for (int j = 0; j < 10; j++) {
			if ((pong.ball.getPosition ().y - pong.ballRadius <= 25.f * (j + 1)) && (pong.ball.getPosition ().y - pong.ballRadius >= 25.f * j)) {
				sector = (int)(pong.ball.getPosition ().x / 100);
				int index0 = sector + 6 * j;
				int index1 = sector + 6 * (j + 1);
				int index2 = sector + 6 * j + 1;
				int index3 = sector + 6 * j - 1;
				int index4 = sector + 6 * (j + 1) + 1;
				int index5 = sector + 6 * (j + 1) - 1;
				index4 > 59 ? index4 = 59 : index4;
				index5 < 0 ? index5 = 0 : index5;

				if (((bricks_show[index0] == 1) || (bricks[index0].type == 2 && bricks[index0].life == 2)) &&
					pong.ball.getPosition ().x >= bricks[index0].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x <= bricks[index0].brick.getPosition ().x + 100.f / 2 &&
					pong.ball.getPosition ().y - pong.ballRadius >= bricks[index0].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y - pong.ballRadius <= bricks[index0].brick.getPosition ().y + 25.f / 2) {
					pong.ballAngle = -pong.ballAngle;
					pong.ball.setPosition (pong.ball.getPosition ().x, 25.f * (j + 1) + pong.ballRadius + 0.1f);
					collisionResult (index0);
				} else if (bricks_show[index1] == 1 &&
					pong.ball.getPosition ().x >= bricks[index1].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x <= bricks[index1].brick.getPosition ().x + 100.f / 2 &&
					pong.ball.getPosition ().y + pong.ballRadius >= bricks[index1].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y + pong.ballRadius <= bricks[index1].brick.getPosition ().y + 25.f / 2) {
					pong.ballAngle = -pong.ballAngle;
					pong.ball.setPosition (pong.ball.getPosition ().x, 25.f * (j + 1) - pong.ballRadius - 0.1f);
					collisionResult (index1);
				}  else if ((bricks_show[index2] == 1) &&
					pong.ball.getPosition ().y >= bricks[index2].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y <= bricks[index2].brick.getPosition ().y + 25.f / 2 &&
					pong.ball.getPosition ().x + pong.ballRadius >= bricks[index2].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x + pong.ballRadius <= bricks[index2].brick.getPosition ().x + 100.f / 2) {
					pong.ballAngle = (pi - pong.ballAngle);
					pong.ball.setPosition (bricks[index2].brick.getPosition ().x - 100.f / 2 - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
					collisionResult (index2);
				} else if ((bricks_show[index3] == 1) &&
					pong.ball.getPosition ().y >= bricks[index3].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y <= bricks[index3].brick.getPosition ().y + 25.f / 2 &&
					pong.ball.getPosition ().x - pong.ballRadius >= bricks[index3].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x - pong.ballRadius <= bricks[index3].brick.getPosition ().x + 100.f / 2) {
					pong.ballAngle = -(pi + pong.ballAngle);
					pong.ball.setPosition (bricks[index3].brick.getPosition ().x + 100.f / 2 + pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
					collisionResult (index3);
				} else if ((bricks_show[index4] == 1) &&
					pong.ball.getPosition ().y >= bricks[index4].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y <= bricks[index4].brick.getPosition ().y + 25.f / 2 &&
					pong.ball.getPosition ().x + pong.ballRadius >= bricks[index4].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x + pong.ballRadius <= bricks[index4].brick.getPosition ().x + 100.f / 2) {
					pong.ballAngle = (pi - pong.ballAngle);
					pong.ball.setPosition (bricks[index4].brick.getPosition ().x - 100.f / 2 - pong.ballRadius - 0.1f, pong.ball.getPosition ().y);
					collisionResult (index4);
				} else if ((bricks_show[index5] == 1) &&
					pong.ball.getPosition ().y >= bricks[index5].brick.getPosition ().y - 25.f / 2 &&
					pong.ball.getPosition ().y <= bricks[index5].brick.getPosition ().y + 25.f / 2 &&
					pong.ball.getPosition ().x - pong.ballRadius >= bricks[index5].brick.getPosition ().x - 100.f / 2 &&
					pong.ball.getPosition ().x - pong.ballRadius <= bricks[index5].brick.getPosition ().x + 100.f / 2) {
					pong.ballAngle = -(pi + pong.ballAngle);
					pong.ball.setPosition (bricks[index5].brick.getPosition ().x + 100.f / 2 + pong.ballRadius + 0.1f, pong.ball.getPosition ().y);
					collisionResult (index5);
				}
			}
		}
	}
}

int Gameplay::isWin () {
	for (auto i : bricks_show) {
		if (i == 1)
			return 0;
	}
	win_sound.play ();
	return 1;
}

void Gameplay::collisionResult (int index) {
	if (bricks[index].type == 1) {
		if (bricks[index].life == 2) {
			damage_sound.play ();
			// Set broke texture
			bricks[index].brick.setTexture (&bricks[index].brokeTex);
			bricks[index].life--;
		} else if (bricks[index].life <= 1) {
			destroy_sound.play ();
			bricks_show[index] = 0;
			score += 50;
			updateScore ();
		}
	} else if (bricks[index].type == 2) {
		if (bricks[index].life == 2) {
			damage_sound.play ();
			bricks_show[index] = 1;
			// Set broke texture
			bricks[index].brick.setTexture (&bricks[index].brokeTex);
			bricks[index].life--;
		} else if (bricks[index].life <= 1) {
			destroy_sound.play ();
			bricks_show[index] = 0;
			score += 50;
			updateScore ();
		}
	} else if (bricks[index].type == 0) {
		destroy_sound.play ();
		bricks_show[index] = 0;
		score += 10;
		updateScore ();
	}

	// Go to the next level, speed would be increased 100 if ball speed is lower than 800
	if (gameState == M1L1 && isWin ()) {
		gameState = MODE10;
		restart ();
		bricks.clear ();
		level0 ();
		level2 ();
		pong.ballSpeed < 800 ? pong.ballSpeed += 100 : pong.ballSpeed = 800;
	} else if (gameState == M1L2 && isWin ()) {
		gameState = MODE1;
		restart ();
		bricks.clear ();
		level0 ();
		level1 ();
		pong.ballSpeed < 800 ? pong.ballSpeed += 100 : pong.ballSpeed = 800;
	} else if (gameState == M2L1 && isWin ()) {
		gameState = MODE20;
		restart ();
		bricks.clear ();
		level0 ();
		level2 ();
		pong.ballSpeed < 800 ? pong.ballSpeed += 100 : pong.ballSpeed = 800;
	} else if (gameState == M2L2 && isWin ()) {
		gameState = MODE2;
		restart ();
		bricks.clear ();
		level0 ();
		level1 ();
		pong.ballSpeed < 800 ? pong.ballSpeed += 100 : pong.ballSpeed = 800;
	}
}