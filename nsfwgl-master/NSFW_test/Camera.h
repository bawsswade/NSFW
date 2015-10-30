#pragma once
#include <nsfw.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>


struct Camera
{
	glm::mat4 transform;
	float a_near, a_far, aspect, fov;

	Camera() : a_near(1), a_far(100), aspect(800/600.f),fov(90) {}

	void update() {}

	void lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
	{
		transform = glm::inverse(glm::lookAt(pos, target, up));
	}

	glm::mat4 getView()			const { return glm::inverse(transform); }
	glm::mat4 getProjection()	const { return glm::perspective(fov, aspect, a_near, a_far); }
};