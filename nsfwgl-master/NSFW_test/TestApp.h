#pragma once

#include "GameObject.h"
#include "FowardPass.h"
#include "PostPass.h"
#include "Camera.h"
#include <nsfw.h>

class TestApp:public nsfw::Application
{
	Camera camera;
	GameObject obj;
	GameObject plane;
	FowardPass fp;
	PostPass cp;

public:
	void onStep();
	void onPlay();
	void onInit();
	void onTerm();
};