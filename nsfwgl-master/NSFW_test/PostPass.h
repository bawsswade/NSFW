#pragma once
#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <nsfw.h>



class PostPass : public nsfw::RenderPass
{
public:
	void prep()
	{
		glUseProgram(*shader);
	}

	void post()
	{
		glUseProgram(0);
	}

	void draw()
	{
		nsfw::Asset<nsfw::ASSET::TEXTURE> fin = "ForwardFinal";
		nsfw::Asset<nsfw::ASSET::TEXTURE> din = "ForwardDepth";
		nsfw::Asset<nsfw::ASSET::TEXTURE> pin = "Particles";
		nsfw::Asset<nsfw::ASSET::TEXTURE> pid = "ParticleDepth";

		nsfw::Asset<nsfw::ASSET::VAO> qm = "Quad";
		nsfw::Asset<nsfw::ASSET::SIZE> qt = "Quad";

		setUniform("Final", nsfw::UNIFORM::TEX2, &fin, 0);
		setUniform("Depth", nsfw::UNIFORM::TEX2, &din, 1);
		setUniform("Particles", nsfw::UNIFORM::TEX2, &pin, 2);
		setUniform("ParticleDepth", nsfw::UNIFORM::TEX2, &pid, 3);

		glBindVertexArray(*qm);
		glDrawElements(GL_TRIANGLES, *qt, GL_UNSIGNED_INT, 0);
	}
};