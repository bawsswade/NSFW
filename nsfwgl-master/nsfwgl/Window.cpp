#include "nsfw.h"

void nsfw::Window::init(unsigned a_width, unsigned a_height)
{
	//TODO_D("Should create and set an active windowing context. ONLY GLFW! No GL!");
	// initialize
	glfwInit();
	
	width = a_width;
	height = a_height;
	window = glfwCreateWindow(width, height, "NSFWGL", NULL, NULL);
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
	currentTime = getTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	

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

float nsfw::Window::getDeltaTime()
{
	return deltaTime;
}

bool nsfw::Window::getKey(unsigned k) const
{
	return glfwGetKey(window, k);
}

bool nsfw::Window::getShouldClose() const
{
	return (glfwWindowShouldClose(window));
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
	//TODO_D("Implemented, not tested.");

	glm::vec3 texelSize = 1.0f/glm::vec3(width,height,0);
	glm::vec3 halfTexel = texelSize * 0.5f; // bottom left

	// scale up to the appropriate size, then shift it to line up w/center of texels
	return glm::translate(halfTexel) * glm::scale(glm::vec3(1, 1, 1) - texelSize);	
}

glm::vec2 nsfw::Window::getCursorPos()
{
	glfwGetCursorPos(window, &currentXCursor, &currentYCursor);
	return glm::vec2(currentXCursor, currentYCursor);
}

int	nsfw::Window::getMouseButton()
{
	if (glfwGetMouseButton(window, 0)) return 0;
	else if(glfwGetMouseButton(window, 1)) return 1;
	else if(glfwGetMouseButton(window, 2)) return 2;
}