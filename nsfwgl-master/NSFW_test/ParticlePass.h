#pragma once
#include <nsfw.h>
#include "Particles.h"
#include "Camera.h"

class ParticlePass : public nsfw::RenderPass
{
public:
	void prep()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
		//glUseProgram(*shader);

		//glClearColor(.2f, .2f, .2f, 1);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void post()
	{
		glDisable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}

	void draw(ParticleEmitter *m_emitter, Camera c)
	{
		setUniform("Projection", nsfw::UNIFORM::MAT4, glm::value_ptr(c.getProjection()));
		setUniform("View", nsfw::UNIFORM::MAT4, glm::value_ptr(c.getView()));

		
		// particle emitter
		m_emitter->Update(nsfw::Window::instance().getDeltaTime(), c.getWorldTransform());
		m_emitter->Draw();
	}

	void GPUdraw(GPUParticleEmitter *e, Camera c)
	{
		e->Draw(nsfw::Window::instance().getTime(), c.getWorldTransform(), c.getProjectionView());
	}
};
