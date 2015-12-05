#pragma once

#include "GameObject.h"
#include "FowardPass.h"
#include "PostPass.h"
#include "ShadowPass.h"
#include "Camera.h"
#include "ParticlePass.h"
#include <nsfw.h>

class TestApp:public nsfw::Application
{
	GPUParticleEmitter *m_GPUemitter;
	ParticleEmitter *m_emitter;
	Camera camera;
	GameObject obj;
	GameObject plane;
	ShadowPass sp;
	ParticlePass pp;
	FowardPass fp;
	Light light;
	Light l2;
	PostPass cp;

public:
	void onStep();
	void onPlay();
	void onInit();
	void onTerm();
};