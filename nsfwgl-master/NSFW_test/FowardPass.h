#pragma once
#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <nsfw.h>

#include "Camera.h"
#include "GameObject.h"
#include "Particles.h"
#include "Light.h"

class FowardPass : public nsfw::RenderPass
{

public:
	nsfw::Asset<nsfw::ASSET::TEXTURE> shadowMap;

	void prep()
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, *fbo);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *shadowMap ,0);

		glUseProgram(*shader);

		glClearColor(0.1f, 0.1f, 0.1f, 1);
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

		float t = nsfw::Window::instance().getTime();
		setUniform("time", nsfw::UNIFORM::FLO1, &(t));
		//int loc = glGetUniformLocation(, "time");
		//glUniform1f(loc, nsfw::Window::instance().getTime());

		glBindVertexArray(*go.mesh);
		glDrawElements(GL_TRIANGLES, *go.tris, GL_UNSIGNED_INT, 0);

		// particle emitter

	}
};