#pragma once

#include "../nsfwgl/nsfw.h"

#include "Camera.h"
#include "Geometry.h"

class GPass : public nsfw::RenderPass
{


public:	
	void prep() 
	{ 
		//TODO_D("glUseProgram, glClear, glBindFrameBuffer, glViewPort, glEnable etc..."); 
		glUseProgram(*shader);
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

		glViewport(0, 0, 512, 512);
		glClearColor(0.2f, 0.2f, 0.2f, 1); 
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void post() 
	{ 
		//TODO_D("Unset any gl settings");
		glViewport(0, 0, 800, 600);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}

	GPass(const char *shaderName, const char *fboName) { shader = shaderName; fbo = fboName; }

	void draw(const Camera &c, const Geometry &g)
	{
		setUniform("Projection",	nsfw::UNIFORM::TYPE::MAT4, glm::value_ptr(c.getProjection()));
		setUniform("View",			nsfw::UNIFORM::TYPE::MAT4, glm::value_ptr(c.getView()));
		setUniform("Model",			nsfw::UNIFORM::TYPE::MAT4, glm::value_ptr(g.transform));

		setUniform("Diffuse",		nsfw::UNIFORM::TEX2,  &g.diffuse,  0);
		setUniform("Normal",		nsfw::UNIFORM::TEX2,  &g.normal,   1);
		setUniform("Specular",		nsfw::UNIFORM::TEX2,  &g.specular, 2);

		setUniform("SpecularPower", nsfw::UNIFORM::FLO1, (void*)&g.specPower);

		// vao name is different than g.mesh
        unsigned GMeshHandle = nsfw::Assets::instance().get(g.mesh);
		unsigned GTrisHandle = nsfw::Assets::instance().get(g.tris);

		glBindVertexArray(GMeshHandle);
		glDrawElements(GL_TRIANGLES, GTrisHandle, GL_UNSIGNED_INT, 0);
		//TODO_D("bindVAO and Draw Elements!");
	}
};