#include "Camera.h"

#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include <glm\ext.hpp>

glm::vec3 Camera::getViewPosition() const
{
	return (inverse(worldTransform))[3].xyz;
}

void Camera::setPosition(vec3 pos)
{
	worldTransform[3].xyz = pos;
}  

void Camera::setPersp(float fov, float aspect, float a_near, float a_far)
{
	worldTransform = glm::perspective(fov, aspect, a_near, a_far);
}

void Camera::lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
{
	worldTransform = glm::inverse(glm::lookAt(pos, target, up));
}

void Camera::Input(float speed, float deltaTime)
{
	//glfwSetScrollCallback(window, scrollFunc);

	/*if (Y_SCROLL == -1)
	{
		worldTransform[3].xyz += worldTransform[2].xyz * speed * deltaTime * 100;
		Y_SCROLL = 0;
	}
	if (Y_SCROLL == 1)
	{
		worldTransform[3].xyz -= worldTransform[2].xyz * speed * deltaTime * 100;
		Y_SCROLL = 0;
	}*/
	vec2 cursorPos = nsfw::Window::instance().getCursorPos();

	deltaXCursor = cursorPos.x - lastXCursor;
	deltaYCursor = cursorPos.y - lastYCursor;
	lastXCursor = cursorPos.x;
	lastYCursor = cursorPos.y;

	// scrollbar click
	if (nsfw::Window::instance().getMouseButton() == 2)
	{
		if (deltaXCursor > 0)
		{
			worldTransform[3] -= worldTransform[0] * speed * deltaXCursor *  deltaTime;
		}
		if (deltaXCursor < 0)
		{
			worldTransform[3] += worldTransform[0] * speed * -deltaXCursor * deltaTime;
		}
		if (deltaYCursor < 0)
		{
			worldTransform[3] -= worldTransform[1] * speed * -deltaYCursor * deltaTime;
		}
		if (deltaYCursor > 0)
		{
			worldTransform[3] += worldTransform[1] * speed * deltaYCursor * deltaTime;
		}
	}

	if (nsfw::Window::instance().getKey('W'))
	{
		worldTransform[3].xyz -= worldTransform[2].xyz * speed * deltaTime;
	}
	if (nsfw::Window::instance().getKey('S'))
	{
		worldTransform[3].xyz += worldTransform[2].xyz * speed * deltaTime;
	}
	if (nsfw::Window::instance().getKey('A'))
	{
		worldTransform[3] -= worldTransform[0] * speed * deltaTime;
	}
	if (nsfw::Window::instance().getKey('D'))
	{
		worldTransform[3] += worldTransform[0] * speed * deltaTime;
	}


	// right click
	if (nsfw::Window::instance().getMouseButton() == 1)
	{
		mat4 rotationX, rotationY;
		float angleX = -deltaXCursor * deltaTime * speed * 10.0f;
		float angleY = -deltaYCursor * deltaTime * speed * 10.0f;

		rotationX *= glm::rotate(angleX, vec3(0, 1, 0));	// horizontal mouse should change yaw

		worldTransform[0] = rotationX * worldTransform[0];
		worldTransform[1] = rotationX * worldTransform[1];
		worldTransform[2] = rotationX * worldTransform[2];

		rotationY *= glm::rotate(angleY, vec3(worldTransform[0].xyz));	// vertical mouse should change pitch

		worldTransform[0] = rotationY * worldTransform[0];
		worldTransform[1] = rotationY * worldTransform[1];
		worldTransform[2] = rotationY * worldTransform[2];
	}
}