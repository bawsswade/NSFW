#include "TestApp.h"

void TestApp::onStep()
{
	fp.prep();
	fp.draw(camera, obj);
	fp.post();

	cp.prep();
	cp.draw();
	cp.post();
}

void TestApp::onPlay()
{
	camera.lookAt(glm::vec3(3, 3, 3),		// offset  
					glm::vec3(0, 0, 0),		// origin
					glm::vec3(0, 1, 0));	// up


	obj.transform = glm::mat4(1);
	obj.diffuse = "SoulSpearsoulspear_diffuse.tga";										// default
	obj.mesh = "SoulSpear_Low:SoulSpear_Low1";						// default
	obj.tris = "SoulSpear_Low:SoulSpear_Low1";						// default

	fp.shader = "Basic";
	fp.fbo = "Forward";

	cp.shader = "Post";
	//fp.fbo = "Screen";						// default
}

void TestApp::onInit()
{
	nsfw::Assets::instance().loadFBX("SoulSpear", "../resources/FBX/soulspear/soulspear.fbx");

	nsfw::Assets::instance().loadShader("Basic","../resources/shaders/basic.vert", 
												"../resources/shaders/basic.frag");

	nsfw::Assets::instance().loadShader("Post", "../resources/shaders/post.vert",
												 "../resources/shaders/post.frag");

	const char   *names[] = { "ForwardFinal", "ForwardDepth" };
	const unsigned deps[] = { GL_RGBA, GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("Forward", 800, 600, 2, names, deps);
}

void TestApp::onTerm()
{

}