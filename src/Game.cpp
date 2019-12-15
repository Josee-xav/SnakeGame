#include "Game.h"
#include <chrono>
#include <iostream>

Game::Game()
	: m_CurrentDirection{ Direction::RIGHT }, m_NextDirection{ Direction::RIGHT }, m_NextDirectionChangeble{ true }, snakeSpeed{ 8.0f }
{
	reset();
}

void Game::update(float elapsedTime)
{
	// Update time values
	static float time = elapsedTime;
	time += elapsedTime;

	// if the snake size is equal to 8 it changes the snake speed to so
	if(m_Snake.size() == 8)
		snakeSpeed = 15.0f;

	// This updates gamestate every 8 times a second for the default. And then if the snake size is 8 then its update gamestate changes to 15 times a second making the snake FasT aS FuCk BoI
	if(time >= 1.0f / snakeSpeed)
	{
		time -= 1.0f / snakeSpeed;

		updateDirection();

		bool shouldReset = !moveSnake();
		if(shouldReset)
			reset();
	}
}

void Game::submitDataToRenderer(TileRenderer* pRenderer)
{
	for(int y = 0; y < s_MapHeight; ++y)
	{
		for(int x = 0; x < s_MapWidth; ++x)
		{
			Tile tile;

			switch(m_pMap[y][x])
			{
				case CellState::EMPTY:
					tile.TileName = "Empty";
					tile.position = glm::vec2(s_MapWidth * x, s_MapHeight * y);
					tile.sidelength = s_SnakeTileSidelength;
					tile.color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
					break;
				case CellState::SNAKE:
					tile.TileName = "Snake";
					tile.position = glm::vec2(s_MapWidth * x, s_MapHeight * y);
					tile.sidelength = s_SnakeTileSidelength;
					tile.color = s_SnakeColor;
					break;
				case CellState::FRUIT:
					tile.TileName = "Fruit";
					tile.position = glm::vec2(s_MapWidth * x, s_MapHeight * y);
					tile.sidelength = s_FruitTileSidelength;
					tile.color = s_FruitColor;
					break;
			}

			pRenderer->submit(tile);
		}
	}
}

void Game::setNextDirection(Direction direction)
{
	if(!m_NextDirectionChangeble)
		return;
	int currentDirection = static_cast<int>(m_CurrentDirection);
	int nextDirection = static_cast<int>(direction);

	int possibleDirections[2] = {
		(currentDirection + 1) % 4,
		(currentDirection + 3) % 4
	};

	if(nextDirection == possibleDirections[0] || nextDirection == possibleDirections[1])
	{
		m_NextDirection = direction;
		m_NextDirectionChangeble = false;
	}
}

void Game::reset()
{
	m_CurrentDirection = Direction::RIGHT;
	m_NextDirection = Direction::RIGHT;
	m_NextDirectionChangeble = true;
	snakeSpeed = 8.0f;
	for(int y = 0; y < s_MapHeight; ++y) //
	{
		for(int x = 0; x < s_MapWidth; ++x)
		{
			m_pMap[y][x] = CellState::EMPTY;
		}
	}
	while(!m_Snake.empty())
		m_Snake.pop();
	for(int x = 6; x < 9; ++x)
	{
		m_Snake.push(glm::vec2(x, 11));
		m_pMap[11][x] = CellState::SNAKE; // snake reset spawn is 11 high and
	}
	spawnFruit();
}

// moves the positions by just incrementing either the x or y position depending on the direction choosen
bool Game::moveSnake()
{
	bool SnakeGrowing = false;

	int headY = m_Snake.back().y;
	int headX = m_Snake.back().x;

	std::cout << ":: X : " << headX << "             ";

	std::cout << ":: Y : " << headY << "\n";

	switch(m_CurrentDirection)
	{
		case Direction::UP:
			headY++;
			if(headY >= s_MapHeight)
				return false;
			break;
		case Direction::RIGHT:
			headX++;
			if(headX >= s_MapWidth)
				return false;
			break;
		case Direction::DOWN:
			headY--;
			if(headY < 0)
				return false;
			break;
		case Direction::LEFT:
			headX--;
			if(headX < 0)
				return false;
			break;
	}

	// Check Cell which holds the new head
	switch(m_pMap[headY][headX])
	{
		case CellState::SNAKE:
			return false;
		case CellState::FRUIT:
			SnakeGrowing = true;
			break;
	}

	m_Snake.push(glm::vec2(headX, headY));
	m_pMap[headY][headX] = CellState::SNAKE;

	/* Pop Tail */
	if(!SnakeGrowing)
	{
		int tailY = m_Snake.front().y;
		int tailX = m_Snake.front().x;
		m_Snake.pop();
		m_pMap[tailY][tailX] = CellState::EMPTY;
	}
	else
		spawnFruit();

	return true;
}

void Game::spawnFruit()
{
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
	unsigned int seed = static_cast<unsigned int>(duration);
	srand(seed);
	int fruitX, fruitY;
	do
	{
		fruitX = rand() % (s_MapWidth);
		fruitY = rand() % (s_MapHeight);
	} while(m_pMap[fruitY][fruitX] != CellState::EMPTY);
	m_pMap[fruitY][fruitX] = CellState::FRUIT;
}
