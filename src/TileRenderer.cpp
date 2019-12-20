#include "TileRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <GL\glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Tile::Tile()
	: position{ glm::vec2(0, 0) }, sidelength{ 10.0f }, color{ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) }
{}

Tile::Tile(glm::vec2 position, float sidelength, glm::vec4 color)
	: position{ position }, sidelength{ sidelength }, color{ color }
{}


void TileRenderer::init()
{
	initVaoAndEbo();
	initShader();
	getUniformLocations();
}

TileRenderer::~TileRenderer()
{
	glDeleteBuffers(1, &s_Vbo);
	glDeleteBuffers(1, &s_Ebo);
	glDeleteVertexArrays(1, &s_Vao);
	glDeleteProgram(s_Shader);
}

// This renders the snake blocks and then fruit block and the empty blocks
void TileRenderer::render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(s_Shader);
	glBindVertexArray(s_Vao);

	// renders each tile
	for(Tile tile : m_SubmittedTiles)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(tile.position + glm::vec2(20, 20), 0.0f)); // position of the box
		model = glm::scale(model, glm::vec3(tile.sidelength / 2.0f));
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // camera matrix ???????
		glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
		//                     ortho(x left,x right, y bottom, y top,z depth shit,z depth shit)

		glUniform4f(s_UniformColorLocation, tile.color.r, tile.color.g, tile.color.b, tile.color.a);
		glUniformMatrix4fv(s_UniformModelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(s_UniformViewLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(s_UniformProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

		if(tile.TileName == "Empty") // if the tile is the empty tile then it changes it to GL_Line else makes sure that its
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // line mode!
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // line mode!

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	m_SubmittedTiles.clear();
}

// initializes vao and ebo with the vertex data to make a square this allows making the snake and the fruit
void TileRenderer::initVaoAndEbo()
{
	float pVertexData[8] = {
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f
	};
	unsigned int pIndexData[6] = {
		0, 1, 2,
		0, 2, 3
	};
	
	glGenVertexArrays(1, &s_Vao);
	glGenBuffers(1, &s_Vbo);
	glGenBuffers(1, &s_Ebo);

	glBindVertexArray(s_Vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_Vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), pVertexData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), pIndexData, GL_STATIC_DRAW);

	glBindVertexArray(0);
}


void TileRenderer::initShader()
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* VertexShaderSrc{
		"#version 460 core\n"
		"layout(location = 0) in vec2 v_Pos;\n"
		"uniform mat4 u_Model;\n"
		"uniform mat4 u_View;\n"
		"uniform mat4 u_Projection;\n"
		"void main() {\n"
		"	gl_Position = u_Projection * u_View * u_Model * vec4(v_Pos, 0.0, 1.0);\n"
		"}\n"
	};
	const char* FragmentShaderSrc{
		"#version 460 core\n"
		"uniform vec4 u_Color;\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = u_Color;"
		"}\n"
	};

	compileShader(vertexShader, &VertexShaderSrc);
	compileShader(fragmentShader, &FragmentShaderSrc);
	linkProgram(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void TileRenderer::compileShader(unsigned int shader, const char** ppShaderSrc)
{
	glShaderSource(shader, 1, ppShaderSrc, nullptr);
	glCompileShader(shader);
#ifdef _DEBUG
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		LOG_ERROR("Failed to compile Shader. InfoLog:\n{0}", infoLog);
		__debugbreak();
	}
#endif
}

void TileRenderer::linkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	s_Shader = glCreateProgram();
	glAttachShader(s_Shader, vertexShader);
	glAttachShader(s_Shader, fragmentShader);
	glLinkProgram(s_Shader);

#ifdef _DEBUG
	int success;
	glGetShaderiv(s_Shader, GL_LINK_STATUS, &success);
	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(s_Shader, 512, nullptr, infoLog);
		LOG_ERROR("Failed to link Program. InfoLog:\n{0}", infoLog);
		__debugbreak();
	}
#endif
}

void TileRenderer::getUniformLocations()
{
	s_UniformColorLocation = glGetUniformLocation(s_Shader, "u_Color");
	//LOG_ASSERT(s_UniformColorLocation != -1, "Failed to get Uniform Location from uniform {0}", "u_Color");
	s_UniformModelLocation = glGetUniformLocation(s_Shader, "u_Model");
	//LOG_ASSERT(s_UniformModelLocation != -1, "Failed to get Uniform Location from uniform {0}", "u_Model");
	s_UniformViewLocation = glGetUniformLocation(s_Shader, "u_View");
	//LOG_ASSERT(s_UniformViewLocation != -1, "Failed to get Uniform Location from uniform {0}", "u_View");
	s_UniformProjectionLocation = glGetUniformLocation(s_Shader, "u_Projection");
	//LOG_ASSERT(s_UniformProjectionLocation != -1, "Failed to get Uniform Location from uniform {0}", "u_Projection");
}

unsigned int TileRenderer::s_Vao;
unsigned int TileRenderer::s_Vbo;
unsigned int TileRenderer::s_Ebo;
unsigned int TileRenderer::s_Shader;
unsigned int TileRenderer::s_UniformColorLocation;
unsigned int TileRenderer::s_UniformModelLocation;
unsigned int TileRenderer::s_UniformViewLocation;
unsigned int TileRenderer::s_UniformProjectionLocation;
