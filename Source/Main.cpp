#include "SFML/Graphics.hpp"

#include "Game.hpp"

#include <fstream>

int main()
{
	const int         WINDOW_WIDTH  = 800;
	const int         WINDOW_HEIGHT = 800;
	const std::string WINDOW_TITLE  = "Flow Field Pathfinder";

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Close);

	std::ifstream settingsFile("settings.txt");

	std::string strGridSize, strEnemySpeed;

	std::getline(settingsFile, strGridSize);
	std::getline(settingsFile, strEnemySpeed);

	Game game(window, std::stoi(strGridSize), std::stof(strEnemySpeed));

	while (window.isOpen())
	{
		game.handleEvents();

		game.update();

		game.render();
	}
}