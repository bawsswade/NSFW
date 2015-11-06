#pragma once
#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <nsfw.h>

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"

class FowardPass : public nsfw::RenderPass
{

public:
	nsfw::Asset<nsfw::ASSET::TEXTURE> shadowMap;

	void prep()
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
		glUseProgram(*shader);

		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void post()
	{
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}

	void draw(const Camera &c, const GameObject &go, const Light &l)
	{
		// Camera
		setUniform("Projection", nsfw::UNIFORM::MAT4, glm::value_ptr(c.getProjection()));
		setUniform("View", nsfw::UNIFORM::MAT4, glm::value_ptr(c.getView()));
		// GameObject
		setUniform("Model", nsfw::UNIFORM::MAT4, glm::value_ptr(go.transform));
		setUniform("Diffuse", nsfw::UNIFORM::TEX2, &(go.diffuse), 1);

		// for shadow texture
		setUniform("shadowMap", nsfw::UNIFORM::TEX2, shadowMap, 0);

		// for light 
		setUniform("LightMatrix", nsfw::UNIFORM::MAT4, glm::value_ptr(l.getProjectionView()));
		setUniform("LightOffset", nsfw::UNIFORM::MAT4, glm::value_ptr(l.getOffset()));
		//setUniform("LightColor", nsfw::UNIFORM::, glm::value_ptr());

		glBindVertexArray(*go.mesh);
		glDrawElements(GL_TRIANGLES, *go.tris, GL_UNSIGNED_INT, 0);
	}
};