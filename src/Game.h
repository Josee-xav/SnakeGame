#pragma once

#include "TileRenderer.h"

#include <queue>


enum class Direction
{
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};

enum class CellState
{
	EMPTY = 0,
	SNAKE = 1,
	FRUIT = 2,
	BORDER = 3,
};

class Game
{
public:
	Game();
	Game(const Game&) = default;
	~Game() = default;

	Game& operator=(const Game&) = default;

	void update(float elapsedTime);
	void submitDataToRenderer(TileRenderer* pRenderer);
	void setNextDirection(Direction direction);

private:
	void reset();

	bool moveSnake();
	void spawnFruit();

	inline void updateDirection()
	{
		m_CurrentDirection = m_NextDirection;
		m_NextDirectionChangeble = true;
	}

	static constexpr int s_MapWidth = 24;
	static constexpr int s_MapHeight = 24;
	static constexpr glm::vec4 s_SnakeColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	static constexpr float s_SnakeTileSidelength = 24.0f;
	static constexpr glm::vec4 s_FruitColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	static constexpr float s_FruitTileSidelength = 14.0f;

	Direction m_CurrentDirection;
	Direction m_NextDirection;
	bool m_NextDirectionChangeble;
	CellState m_pMap[s_MapHeight][s_MapWidth];
	std::queue<glm::ivec2> m_Snake;

	float snakeSpeed;
};

