#include "nsfw.h"

void nsfw::Window::init(unsigned width, unsigned height)
{
	//TODO_D("Should create and set an active windowing context. ONLY GLFW! No GL!");
	// initialize
	glfwInit();
	window = glfwCreateWindow(width, height, NULL, NULL);
	glfwMakeContextCurrent(window);
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);
}

void nsfw::Window::step()
{
	//TODO_D("GLFW poll events and swap buffers is all that should really be here! No GL!");
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void nsfw::Window::term()
{
	glfwTerminate();
}

float nsfw::Window::getTime() const
{
	return glfwGetTime();
}

bool nsfw::Window::getKey(unsigned k) const
{
	return glfwGetKey(window, k);
}

bool nsfw::Window::getShouldClose() const
{
	return (!glfwWindowShouldClose(window));
}

unsigned nsfw::Window::getWidth() const
{
	return width;
}

unsigned nsfw::Window::getHeight() const
{
	return height;
}

glm::mat4 nsfw::Window::getTexelAdjustmentMatrix() const
{
	TODO_D("Implemented, not tested.");

	glm::vec3 texelSize = 1.0f/glm::vec3(width,height,0);
	glm::vec3 halfTexel = texelSize * 0.5f; // bottom left

	// scale up to the appropriate size, then shift it to line up w/center of texels
	return glm::translate(halfTexel) * glm::scale(glm::vec3(1, 1, 1) - texelSize);	
}