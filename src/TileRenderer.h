#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

constexpr float width(600.0f);
constexpr float height(700.0f);

struct Tile
{
	Tile();
	Tile(glm::vec2 position, float sidelength, glm::vec4 color);
	glm::vec2 position;
	std::string TileName;
	float sidelength;
	glm::vec4 color;

};

class TileRenderer
{
public:
	TileRenderer() = default;
	~TileRenderer();

	TileRenderer& operator=(const TileRenderer&) = default;

	static void init();

	inline void submit(const Tile& tile) { m_SubmittedTiles.push_back(tile); }

	void render();

private:
	static void initVaoAndEbo();
	static void initShader();
	static void compileShader(unsigned int shader, const char** ppShaderSrc);
	static void linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
	static void getUniformLocations();


	std::vector<Tile> m_SubmittedTiles;

	// opengl vars
	static unsigned int s_Vao;
	static unsigned int s_Vbo;
	static unsigned int s_Ebo;
	static unsigned int s_Shader;
	static unsigned int s_UniformColorLocation;
	static unsigned int s_UniformModelLocation;
	static unsigned int s_UniformViewLocation;
	static unsigned int s_UniformProjectionLocation;
};

