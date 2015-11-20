#pragma once
#include <nsfw.h>
#include "Light.h"
#include "GameObject.h"

class ShadowPass : public nsfw::RenderPass
{
public:


	void prep() 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
		glUseProgram(*shader);

		glViewport(0, 0, 512, 512);
		glClearColor(.2f, .2f, .2f, 1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glClear(GL_DEPTH_BUFFER_BIT);

	}

	void post() 
	{
		glViewport(0, 0, 800, 600);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}

	void draw(const Light &l, const GameObject &o)
	{
	
		// set projection from light source
		setUniform("projection", nsfw::UNIFORM::MAT4, glm::value_ptr(l.getProjection()));
		setUniform("view", nsfw::UNIFORM::MAT4, glm::value_ptr(l.getView()));
		setUniform("model", nsfw::UNIFORM::MAT4, glm::value_ptr(o.transform));

		glBindVertexArray(*o.mesh);
		glDrawElements(GL_TRIANGLES, *o.tris, GL_UNSIGNED_INT, 0);
	}
};