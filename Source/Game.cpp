#include "Game.hpp"

Game::Game(sf::RenderWindow& window, const int GRID_SIZE, const int ENEMY_SPEED)
	: window(window), GRID_SIZE(GRID_SIZE), ENEMY_SPEED(ENEMY_SPEED), CELL_SIZE(window.getSize().x / GRID_SIZE)
{
	for (int y = 0; y < GRID_SIZE; y++) {
		std::vector<int> row;

		for (int x = 0; x < GRID_SIZE; x++) {
			row.push_back(0);
		}

		walls.push_back(row);
	}

	calculatePath();

	font.loadFromFile("assets/RobotoMono-Medium.ttf");
}

void Game::handleEvents()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::Closed:
				window.close();

				break;
			case sf::Event::Resized: {
				sf::View view = window.getView();

				view.setSize((sf::Vector2f) window.getSize());

				window.setView(view);
			}
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
					isAddingWalls = true;
				else if (event.mouseButton.button == sf::Mouse::Right)
					isRemovingWalls = true;
				
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
					isAddingWalls = false;
				else if (event.mouseButton.button == sf::Mouse::Right)
					isRemovingWalls = false;

				break;
			case sf::Event::KeyReleased:
				if (event.key.code == sf::Keyboard::Num0) {
					areExtraFourTilesEnabled = !areExtraFourTilesEnabled;

					calculatePath();
				} else if (event.key.code == sf::Keyboard::E) {
					enemies.push_back((sf::Vector2f) sf::Mouse::getPosition(window));
				} else if (event.key.code == sf::Keyboard::Backspace) {
					areEnemiesMoving = !areEnemiesMoving;
				}
		}
	}
}

void Game::calculateTileDistance(int x, int y, int currentWave)
{
	if (distanceMap[y][x] > currentWave + 1) distanceMap[y][x] = currentWave + 1;
}

void Game::calculateDistanceMap()
{
	distanceMap = walls;

	const int F_INF = GRID_SIZE * GRID_SIZE;

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			distanceMap[y][x] = F_INF;
		}
	}

	distanceMap[goalPosition.y][goalPosition.x] = 0;

	/* Adding these gets rid of the local optima problem, but could cause issues later */
	if (areExtraFourTilesEnabled) {
		distanceMap[goalPosition.y][goalPosition.x + 1] = 0;
		distanceMap[goalPosition.y + 1][goalPosition.x] = 0;
		distanceMap[goalPosition.y + 1][goalPosition.x + 1] = 0;
	}

	int currentWave = 0;

	bool isCurrentWaveFound = true;

	while (isCurrentWaveFound) {
		isCurrentWaveFound = false;

		for (int y = 0; y < GRID_SIZE; y++) {
			for (int x = 0; x < GRID_SIZE; x++) {
				if (distanceMap[y][x] == currentWave) {
					isCurrentWaveFound = true;

					if (x != 0 && walls[y][x - 1] == 0) calculateTileDistance(x - 1, y, currentWave);
					if (x != GRID_SIZE - 1 && walls[y][x + 1] == 0) calculateTileDistance(x + 1, y, currentWave);
					if (y != 0 && walls[y - 1][x] == 0) calculateTileDistance(x, y - 1, currentWave);
					if (y != GRID_SIZE - 1 && walls[y + 1][x] == 0) calculateTileDistance(x, y + 1, currentWave);
				}
			}
		}

		currentWave++;
	}
}

void Game::calculateVectorMap()
{
	vectorMap.clear();

	for (int y = 0; y < GRID_SIZE; y++) {
		std::vector<sf::Vector2f> row;

		for (int x = 0; x < GRID_SIZE; x++) {
			int left, right, top, bottom;

			if (x == 0 || walls[y][x - 1] == 1) left = distanceMap[y][x];
			else left = distanceMap[y][x - 1];

			if (x == GRID_SIZE - 1 || walls[y][x + 1]) right = distanceMap[y][x];
			else right = distanceMap[y][x + 1];

			if (y == 0 || walls[y - 1][x]) top = distanceMap[y][x];
			else top = distanceMap[y - 1][x];

			if (y == GRID_SIZE - 1 || walls[y + 1][x]) bottom = distanceMap[y][x];
			else bottom = distanceMap[y + 1][x];

			row.push_back(sf::Vector2f(left - right, top - bottom));
		}

		vectorMap.push_back(row);
	}
}

void Game::calculatePath()
{
	calculateDistanceMap();
	calculateVectorMap();
}

void Game::update()
{
	static sf::Clock updateClock;

	const float elapsedTime = updateClock.getElapsedTime().asSeconds();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) currentDrawMode = DrawMode::NONE;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) currentDrawMode = DrawMode::DISTANCES;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) currentDrawMode = DrawMode::ORIGINAL_VECTORS;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) currentDrawMode = DrawMode::NORMALISED_VECTORS;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) currentDrawMode = DrawMode::ARROWS;

	if (isAddingWalls) {
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		bool isMouseWithinWindow = mousePosition.x > 0 && mousePosition.y > 0 && mousePosition.x < window.getSize().x&& mousePosition.y < window.getSize().y;

		if (isMouseWithinWindow) {
			walls[mousePosition.y / CELL_SIZE][mousePosition.x / CELL_SIZE] = 1;

			calculatePath();
		}
	} else if (isRemovingWalls) {
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		bool isMouseWithinWindow = mousePosition.x > 0 && mousePosition.y > 0 && mousePosition.x < window.getSize().x&& mousePosition.y < window.getSize().y;

		if (isMouseWithinWindow) {
			walls[mousePosition.y / CELL_SIZE][mousePosition.x / CELL_SIZE] = 0;

			calculatePath();
		}
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		bool isMouseWithinWindow = mousePosition.x > 0 && mousePosition.y > 0 && mousePosition.x < window.getSize().x&& mousePosition.y < window.getSize().y;

		if (isMouseWithinWindow) {
			goalPosition = sf::Vector2i(mousePosition.x / CELL_SIZE, mousePosition.y / CELL_SIZE);

			calculatePath();
		}
	}

	if (areEnemiesMoving) {
		for (int i = 0; i < enemies.size(); i++) {
			int enemyLeft = (enemies[i].x - CELL_SIZE * 0.1f) / CELL_SIZE;
			int enemyTop = (enemies[i].y - CELL_SIZE * 0.1f) / CELL_SIZE;
			int enemyRight =(enemies[i].x + CELL_SIZE * 0.1f) / CELL_SIZE;
			int enemyBottom = (enemies[i].y + CELL_SIZE * 0.1f) / CELL_SIZE;

			if (!(enemyLeft == goalPosition.x && enemyTop == goalPosition.y)) {

				sf::Vector2f topLeftVector, topRightVector, bottomLeftVector, bottomRightVector;

				if (enemyLeft > 0 && enemyTop > 0) topLeftVector = vectorMap[enemyTop][enemyLeft];
				if (enemyRight < GRID_SIZE - 1 && enemyTop > 0) topRightVector = vectorMap[enemyTop][enemyRight];
				if (enemyLeft > 0 && enemyBottom < GRID_SIZE - 1) bottomLeftVector = vectorMap[enemyBottom][enemyLeft];
				if (enemyRight < GRID_SIZE - 1 && enemyBottom < GRID_SIZE - 1) bottomRightVector = vectorMap[enemyBottom][enemyRight];

				sf::Vector2f totalVector = topLeftVector + topRightVector + bottomLeftVector + bottomRightVector;

				float vectorMagnitude = std::sqrt(std::pow(totalVector.x, 2) + std::pow(totalVector.y, 2));

				if (vectorMagnitude == 0) vectorMagnitude = 1;

				sf::Vector2f normalisedVector = totalVector / vectorMagnitude;

				sf::FloatRect enemyBounds(0, 0, CELL_SIZE * 0.1f * 2.0f, CELL_SIZE * 0.1f * 2.0f);

				sf::Vector2f originalEnemyPosition = enemies[i];

				enemies[i].x += normalisedVector.x * ENEMY_SPEED * elapsedTime;

				enemyBounds.left = enemies[i].x - enemyBounds.width / 2.0f;
				enemyBounds.top = originalEnemyPosition.y - enemyBounds.height / 2.0f;

				for (int y = 0; y < GRID_SIZE; y++) {
					for (int x = 0; x < GRID_SIZE; x++) {
						if (walls[y][x] == 1) {
							sf::FloatRect wallBounds(CELL_SIZE * x, CELL_SIZE * y, CELL_SIZE, CELL_SIZE);

							if (enemyBounds.intersects(wallBounds)) {
								enemies[i].x = originalEnemyPosition.x;

								goto BREAK_OUT_OF_LOOP_1;
							}
						}
					}
				}
				BREAK_OUT_OF_LOOP_1:

				enemies[i].y += normalisedVector.y * ENEMY_SPEED * elapsedTime;

				enemyBounds.left = originalEnemyPosition.x - enemyBounds.width / 2.0f;
				enemyBounds.top = enemies[i].y - enemyBounds.height / 2.0f;

				for (int y = 0; y < GRID_SIZE; y++) {
					for (int x = 0; x < GRID_SIZE; x++) {
						if (walls[y][x] == 1) {
							sf::FloatRect wallBounds(CELL_SIZE * x, CELL_SIZE * y, CELL_SIZE, CELL_SIZE);

							if (enemyBounds.intersects(wallBounds)) {
								enemies[i].y = originalEnemyPosition.y;

								goto BREAK_OUT_OF_LOOP_2;
							}
						}
					}
				}
				BREAK_OUT_OF_LOOP_2:{}
			}
		}
	}

	updateClock.restart();
}

void Game::drawGrid()
{
	sf::RectangleShape gridLine;

	gridLine.setFillColor(sf::Color(8, 156, 255));

	const int GRID_LINE_THICKNESS = 2;

	for (int i = 0; i < GRID_SIZE; i++) {
		gridLine.setSize(sf::Vector2f(GRID_LINE_THICKNESS, window.getSize().y));
		gridLine.setPosition(sf::Vector2f(CELL_SIZE * i - GRID_LINE_THICKNESS / 2, 0));

		window.draw(gridLine);
	}

	for (int i = 0; i < GRID_SIZE; i++) {
		gridLine.setSize(sf::Vector2f(window.getSize().x, GRID_LINE_THICKNESS));
		gridLine.setPosition(sf::Vector2f(0, CELL_SIZE * i - GRID_LINE_THICKNESS / 2));

		window.draw(gridLine);
	}
}

void Game::drawWalls()
{
	sf::RectangleShape wall;

	wall.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));

	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			if (walls[y][x] == 1) {
				wall.setPosition(sf::Vector2f(CELL_SIZE * x, CELL_SIZE * y));

				window.draw(wall);
			}
		}
	}
}

void Game::drawGoal()
{
	sf::RectangleShape goal;

	goal.setFillColor(sf::Color(50, 255, 149));
	goal.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
	goal.setPosition(sf::Vector2f(CELL_SIZE * goalPosition.x, CELL_SIZE * goalPosition.y));

	window.draw(goal);
}

void Game::drawDistanceMap()
{
	sf::Text distanceText;

	distanceText.setCharacterSize(CELL_SIZE / 7);
	distanceText.setFont(font);

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (distanceMap[y][x] == GRID_SIZE * GRID_SIZE)
				distanceText.setString("INF");
			else
				distanceText.setString(std::to_string(distanceMap[y][x]));
			
			distanceText.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f - distanceText.getLocalBounds().width / 2.0f - distanceText.getLocalBounds().left, CELL_SIZE * y + CELL_SIZE / 2.0f - distanceText.getLocalBounds().height / 2.0f - distanceText.getLocalBounds().top));

			window.draw(distanceText);
		}
	}
}

void Game::drawOriginalVectorMap()
{
	sf::Text xText, yText;

	xText.setCharacterSize(CELL_SIZE / 7);
	xText.setFont(font);

	yText.setCharacterSize(CELL_SIZE / 7);
	yText.setFont(font);

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (distanceMap[y][x] == GRID_SIZE * GRID_SIZE) {
				xText.setString("0");
				yText.setString("0");
			}
			else {
				xText.setString(std::to_string((int) vectorMap[y][x].x));
				yText.setString(std::to_string((int) vectorMap[y][x].y));
			}

			xText.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f - xText.getLocalBounds().width / 2.0f - xText.getLocalBounds().left, CELL_SIZE * y + CELL_SIZE * 0.35f - xText.getLocalBounds().height / 2.0f - xText.getLocalBounds().top));
			yText.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f - yText.getLocalBounds().width / 2.0f - yText.getLocalBounds().left, CELL_SIZE * y + CELL_SIZE * 0.65f - yText.getLocalBounds().height / 2.0f - yText.getLocalBounds().top));

			window.draw(xText);
			window.draw(yText);
		}
	}
}

void Game::drawNormalisedVectorMap()
{
	sf::Text xText, yText;

	xText.setCharacterSize(CELL_SIZE / 7);
	xText.setFont(font);

	yText.setCharacterSize(CELL_SIZE / 7);
	yText.setFont(font);

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (distanceMap[y][x] == GRID_SIZE * GRID_SIZE) {
				xText.setString("0");
				yText.setString("0");
			}
			else {
				float vectorMagnitude = std::sqrt(std::pow(vectorMap[y][x].x, 2) + std::pow(vectorMap[y][x].y, 2));

				xText.setString(std::to_string(vectorMap[y][x].x / vectorMagnitude));
				yText.setString(std::to_string(vectorMap[y][x].y / vectorMagnitude));
			}

			xText.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f - xText.getLocalBounds().width / 2.0f - xText.getLocalBounds().left, CELL_SIZE * y + CELL_SIZE * 0.35f - xText.getLocalBounds().height / 2.0f - xText.getLocalBounds().top));
			yText.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f - yText.getLocalBounds().width / 2.0f - yText.getLocalBounds().left, CELL_SIZE * y + CELL_SIZE * 0.65f - yText.getLocalBounds().height / 2.0f - yText.getLocalBounds().top));

			window.draw(xText);
			window.draw(yText);
		}
	}
}

void Game::drawArrowMap()
{
	sf::RectangleShape mainLine, leftLine, rightLine;

	mainLine.setSize(sf::Vector2f(CELL_SIZE * 0.4f, 2));
	mainLine.setOrigin(sf::Vector2f(mainLine.getSize().x / 2.0f, mainLine.getSize().y / 2.0f));

	leftLine.setSize(sf::Vector2f(CELL_SIZE * 0.2f, 2));
	leftLine.setOrigin(sf::Vector2f(0, mainLine.getSize().y / 2.0f));

	rightLine.setSize(sf::Vector2f(CELL_SIZE * 0.2f, 2));
	rightLine.setOrigin(sf::Vector2f(0, mainLine.getSize().y / 2.0f));


	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (walls[y][x] == 0 && distanceMap[y][x] != GRID_SIZE * GRID_SIZE) {
				mainLine.setPosition(sf::Vector2f(CELL_SIZE * x + CELL_SIZE / 2.0f, CELL_SIZE * y + CELL_SIZE / 2.0f));
				mainLine.setRotation(std::atan2(vectorMap[y][x].y, vectorMap[y][x].x) * 180.0f / 3.14f);

				window.draw(mainLine);

				float vectorMagnitude = std::sqrt(std::pow(vectorMap[y][x].x, 2) + std::pow(vectorMap[y][x].y, 2));

				float normalisedX = vectorMap[y][x].x / vectorMagnitude;
				float normalisedY = vectorMap[y][x].y / vectorMagnitude;

				leftLine.setPosition(sf::Vector2f(mainLine.getPosition().x + normalisedX * mainLine.getSize().x * 0.5f, mainLine.getPosition().y + normalisedY * mainLine.getSize().x * 0.5f));
				leftLine.setRotation(mainLine.getRotation() - 135.0f);

				window.draw(leftLine);

				rightLine.setPosition(sf::Vector2f(mainLine.getPosition().x + normalisedX * mainLine.getSize().x * 0.5f, mainLine.getPosition().y + normalisedY * mainLine.getSize().x * 0.5f));
				rightLine.setRotation(mainLine.getRotation() + 135.0f);

				window.draw(rightLine);
			}
		}
	}
}

void Game::drawEnemies()
{
	sf::CircleShape enemy;

	enemy.setRadius(CELL_SIZE * 0.1f);
	enemy.setOrigin(sf::Vector2f(enemy.getRadius(), enemy.getRadius()));
	enemy.setFillColor(sf::Color(255, 204, 50));

	for (int i = 0; i < enemies.size(); i++) {
		enemy.setPosition(enemies[i]);

		window.draw(enemy);
	}
}

void Game::render()
{
	window.clear(sf::Color(51, 173, 255));

	drawGrid();
	if (currentDrawMode == DrawMode::DISTANCES) drawDistanceMap();
	else if (currentDrawMode == DrawMode::ORIGINAL_VECTORS) drawOriginalVectorMap();
	else if (currentDrawMode == DrawMode::NORMALISED_VECTORS) drawNormalisedVectorMap();
	else if (currentDrawMode == DrawMode::ARROWS) drawArrowMap();
	drawWalls();
	drawGoal();
	drawEnemies();

	window.display();
}