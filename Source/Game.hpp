#pragma once

#include "SFML/Graphics.hpp"

class Game
{
	private:
		sf::RenderWindow& window;

		sf::Font font;

		const int GRID_SIZE;
		const int CELL_SIZE;

		bool areExtraFourTilesEnabled = false;

		bool isAddingWalls = false;
		bool isRemovingWalls = false;

		bool areEnemiesMoving = false;

		const float ENEMY_SPEED;

		std::vector<std::vector<int>> walls;

		sf::Vector2i goalPosition;

		std::vector<std::vector<int>> distanceMap;
		std::vector<std::vector<sf::Vector2f>> vectorMap;

		enum class DrawMode {
			NONE,
			DISTANCES,
			ORIGINAL_VECTORS,
			NORMALISED_VECTORS,
			ARROWS
		};

		DrawMode currentDrawMode = DrawMode::NONE;

		std::vector<sf::Vector2f> enemies;

	public:
		     Game         (sf::RenderWindow&, const int, const int);

		void handleEvents            ();
		void calculateTileDistance   (int, int, int);
		void calculateDistanceMap    ();
		void calculateVectorMap      ();
		void calculatePath           ();
		void update                  ();
		void drawGrid                ();
		void drawWalls               ();
		void drawGoal                ();
		void drawEnemies             ();
		void drawDistanceMap         ();
		void drawOriginalVectorMap   ();
		void drawNormalisedVectorMap ();
		void drawArrowMap            ();
		void render                  ();
};