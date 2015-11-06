#include "TestApp.h"
#include <iostream>
using namespace std;

void TestApp::onStep()
{
	camera.Input(10.0f, nsfw::Window::instance().getDeltaTime());
	float dt = nsfw::Window::instance().getDeltaTime();
	obj.transform = glm::rotate(obj.transform, (dt * 10), vec3(0, 1, 0));

	sp.prep();
	sp.draw(light, obj);
	sp.draw(light, plane);
	//sp.draw(l2, obj);		// trying to add another light but need to have another shader propgram
	//sp.draw(l2, plane);
	sp.post();

	fp.prep();
	fp.draw(camera, obj, light);
	fp.draw(camera, plane, light);
	//fp.draw(camera, obj, l2);
	//fp.draw(camera, plane, l2);
	fp.post();

	cp.prep();
	cp.draw();
	cp.post();
}

void TestApp::onPlay()
{
	light.direction = glm::normalize(vec4(1));
	light.color = vec4(1.0f, 0.1f, 0.1f, 0);
	//l2.direction = glm::normalize(vec4(-1, 1, 1, 1));
	//l2.color = vec4(0.1f, 0.1f, 1.0f, 0);
	obj.diffuse = "SoulSpearsoulspear_diffuse.tga";					// default
	obj.mesh = "SoulSpear_Low:SoulSpear_Low1";						// default
	obj.tris = "SoulSpear_Low:SoulSpear_Low1";						// default

	plane.diffuse = "White";
	plane.mesh = "Quad";
	plane.tris = "Quad";

	plane.transform = glm::scale(vec3(5, 5, 5));
	plane.transform = glm::rotate(plane.transform, 270.0f, vec3(1, 0, 0));

	sp.shader = "ShadowShader";
	sp.fbo = "ShadowFBO";

	fp.shader = "Light";
	fp.fbo = "Forward";
	fp.shadowMap = "ShadowDepth";

	cp.shader = "Post";	
}

void TestApp::onInit()
{
	nsfw::Assets::instance().loadFBX("SoulSpear", "../resources/FBX/soulspear/soulspear.fbx");

	nsfw::Assets::instance().loadShader("Basic","../resources/shaders/basic.vert", "../resources/shaders/basic.frag");

	nsfw::Assets::instance().loadShader("Post", "../resources/shaders/post.vert", "../resources/shaders/post.frag");

	nsfw::Assets::instance().loadShader("Light", "../resources/shaders/light.vert", "../resources/shaders/light.frag");

	nsfw::Assets::instance().loadShader("ShadowShader", "../resources/shaders/shadow.vert", "../resources/shaders/shadow.frag");

	nsfw::Assets::instance().loadTexture("White", "../resources/textures/white.png");

	// foward rendering
	const char   *names[] = { "ForwardFinal", "ForwardDepth" };
	const unsigned deps[] = { GL_RGBA, GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("Forward", 800, 600, 2, names, deps);

	// need to create FBO for shadowMap but save texture as a depth attachment or is above it?
	const char   *s_names[] = {"ShadowDepth" };
	const unsigned s_deps[] = { GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("ShadowFBO", 512, 512, 1, s_names, s_deps);

	camera.lookAt(glm::vec3(3, 3, 3),		// offset  
		glm::vec3(0, 0, 0),		// origin
		glm::vec3(0, 1, 0));	// up
}

void TestApp::onTerm()
{

}