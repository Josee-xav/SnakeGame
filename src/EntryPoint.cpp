#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "TileRenderer.h"
#include "Game.h"
#include <iostream>

namespace gs
{
	static TileRenderer* pRenderer = nullptr;
	static Game game;

	void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
	{
		if(action == GLFW_PRESS)
		{
			switch(key)
			{
				case GLFW_KEY_UP:
					game.setNextDirection(Direction::UP);
					break;
				case GLFW_KEY_RIGHT:
					game.setNextDirection(Direction::RIGHT);
					break;
				case GLFW_KEY_DOWN:
					game.setNextDirection(Direction::DOWN);
					break;
				case GLFW_KEY_LEFT:
					game.setNextDirection(Direction::LEFT);
					break;
			}
		}
	}
}

int main()
{
	bool glfwInitialized = glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* pWindow = glfwCreateWindow(width, height, "SnakeGL", nullptr, nullptr);
	glfwMakeContextCurrent(pWindow);

	if(glewInit()) {
		std::cout << "glew init failed\n";
		return -1;
	}

	glViewport(0, 0, width, height);

	glfwSetKeyCallback(pWindow, gs::keyCallback);

	gs::pRenderer = new TileRenderer();
	gs::pRenderer->init();

	float currentFrameTime = static_cast<float>(glfwGetTime());
	float lastFrameTime = static_cast<float>(glfwGetTime());
	float elapsedTime;

	while(!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();

		lastFrameTime = currentFrameTime;
		currentFrameTime = static_cast<float>(glfwGetTime());
		elapsedTime = currentFrameTime - lastFrameTime;

		gs::game.update(elapsedTime);
		gs::game.submitDataToRenderer(gs::pRenderer);
		gs::pRenderer->render();

		glfwSwapBuffers(pWindow);
	}

	delete gs::pRenderer;

	glfwTerminate();
	system("pause");
}
