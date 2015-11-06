#pragma once
#include <glm\ext.hpp>

struct Light
{
	glm::vec4 direction, color;

	float shadowBias = 10.0f;


	glm::mat4 textureSpaceOffset = mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	glm::mat4 getProjection()		const { return glm::ortho<float>(-10, 10, -10, 10, -10, 10); }
	glm::mat4 getView()				const { return glm::lookAt(glm::vec3(direction), glm::vec3(0), glm::vec3(0, 1, 0)); }
	glm::mat4 getProjectionView()	const { return getProjection() * getView(); }
	glm::mat4 getOffset()			const { return textureSpaceOffset; }
	glm::vec4 getDirection()		const { return direction; }
	glm::vec4 getColor()			const { return color; }
	float getBias()					const { return shadowBias; }
};

/*
m_lightDirection = glm::normalize(glm::vec3(-5, 0.5f, 0));
glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
glm::mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));
m_lightMatrix = lightProjection * lightView;
*/