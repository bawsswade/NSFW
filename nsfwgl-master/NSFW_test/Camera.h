#pragma once

#include <nsfw.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>


struct Camera
{
	//glm::mat4 transform;
	glm::mat4 worldTransform;

	float a_near, a_far, aspect, fov;

	double lastXCursor = 0, lastYCursor = 0;/* , currentXCursor = 0, currentYCursor = 0;*/
	double deltaXCursor = 0, deltaYCursor = 0;

	Camera() : a_near(1), a_far(100), aspect(800/600.f),fov(90) {}

	void update() {}

	void lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
	void setPosition(vec3 pos);
	void setPersp(float fov, float aspect, float a_near, float a_far);

	glm::mat4 getWorldTransform()	const { return worldTransform; }
	glm::mat4 getView()				const { return glm::inverse(worldTransform); }
	glm::mat4 getProjection()		const { return glm::perspective(fov, aspect, a_near, a_far); }
	glm::mat4 getProjectionView()	const { return getProjection() * getView(); }
	glm::vec3 getViewPosition() const;	// IN THE CPP NOT SURE WHy

	void Input(float speed, float deltaTime);

};