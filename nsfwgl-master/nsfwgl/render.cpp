#include "nsfw.h"


// Each Case should set up a uniform using the pass parameters
bool nsfw::RenderPass::setUniform(const char *name, nsfw::UNIFORM::TYPE type, const void *value, unsigned count, bool normalize) 
{
	switch (type)
	{
	case nsfw::UNIFORM::FLO1: /*glUniform1fv(glGetUniformLocation(name), count, (float*)value );*/ TODO_D("Setup float uniform!");     break;
	case nsfw::UNIFORM::FLO3: /*glUniform3fv(glGetUniformLocation(name), count, (float*)value);*/ TODO_D("Setup vec3 uniform!");      break;
	case nsfw::UNIFORM::FLO4: TODO_D("Setup vec4 uniform!");      break;
	case nsfw::UNIFORM::MAT4: TODO_D("Setup mat4 uniform!");	  break;
	case nsfw::UNIFORM::INT1: TODO_D("Setup integer uniform!");	  break;
	case nsfw::UNIFORM::TEX2: TODO_D("Setup texture2D uniform!"); break;
		// glActiveTexture(GL_TEXTURE0 + count);
		// glUniformiv(glGetUniformLocation(name),1, (unsigned*)value);
	default:				  TODO_D("INVALID Uniform type.");	  break;
	}

	return false;
}

// set GL state settings and globally accessible uniforms! Should be called before rendering occurs!
void nsfw::RenderPass::prep() { TODO_D("OVERRIDE: Setup the state of the card for this pass."); }

// reset the GL state- undo what we did just in case.
void nsfw::RenderPass::post() { TODO_D("OVERRIDE: Clean up the state of the card."); }