#pragma once

#include "GameObject.h"
#include "FowardPass.h"
#include "PostPass.h"
#include "ShadowPass.h"
#include "Camera.h"
#include <nsfw.h>

class TestApp:public nsfw::Application
{
	Camera camera;
	GameObject obj;
	GameObject plane;
	ShadowPass sp;
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