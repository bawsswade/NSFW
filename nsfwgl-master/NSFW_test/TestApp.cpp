#include "TestApp.h"
#include <iostream>
using namespace std;

void TestApp::onInit()
{
	nsfw::Assets::instance().loadFBX("SoulSpear", "../resources/FBX/soulspear/soulspear.fbx");

	nsfw::Assets::instance().loadShader("Basic", "../resources/shaders/basic.vert", "../resources/shaders/basic.frag");

	nsfw::Assets::instance().loadShader("Post", "../resources/shaders/post.vert", "../resources/shaders/post.frag");

	nsfw::Assets::instance().loadShader("Light", "../resources/shaders/light.vert", "../resources/shaders/light.frag");

	nsfw::Assets::instance().loadShader("ShadowShader", "../resources/shaders/shadow.vert", "../resources/shaders/shadow.frag");

	//nsfw::Assets::instance().loadShader("ParticleShader", "../resources/shaders/particle.vert", "../resources/shaders/particle.frag");

	// for GPU particles
	nsfw::Assets::instance().loadShader("GPUParticleShader", "../resources/shaders/gpuParticle.vert", "../resources/shaders/gpuParticle.frag", "../resources/shaders/gpuParticle.geom");

	nsfw::Assets::instance().loadTexture("Stone", "../resources/textures/stone.jpg");

	nsfw::Assets::instance().loadTexture("White", "../resources/textures/white.png");

	// foward rendering
	const char   *names[] = { "ForwardFinal", "ForwardDepth" };
	const unsigned deps[] = { GL_RGBA, GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("Forward", 800, 600, 2, names, deps);

	// for particles
	const char *p_names[] = { "Particles", "ParticleDepth" };
	const unsigned p_deps[] = { GL_RGBA, GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("ParticlesFBO", 800, 600, 2, p_names, p_deps);

	// for GPUParticles
	//const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	// something like this
	//glTransformFeedbackVaryings(nsfw::Assets::instance().get("GPUParticleShader"), 4, varyings, GL_INTERLEAVED_ATTRIBS);


	// shadow
	const char   *s_names[] = { "ShadowDepth" };
	const unsigned s_deps[] = { GL_DEPTH_COMPONENT };
	nsfw::Assets::instance().makeFBO("ShadowFBO", 512, 512, 1, s_names, s_deps);

	camera.lookAt(glm::vec3(3, 3, 3),		// offset  
		glm::vec3(0, 0, 0),		// origin
		glm::vec3(0, 1, 0));	// up

	// GPU Particle Emitter
	m_GPUemitter = new GPUParticleEmitter();
	m_GPUemitter->Initialize(100000, 0.1f, 5.0f, 5, 20, 1, 0.1f, vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));

	// particle emitter
	m_emitter = new ParticleEmitter();
	m_emitter->Create(1000, 500, 0.1f, 2.0f, 1, 3, 0.3f, 0.05f, vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));
}

void TestApp::onPlay()
{
	light.direction = glm::normalize(vec4(1));
	light.color = vec4(0.3f, 0.3f, 0.3f, 0);
	//l2.direction = glm::normalize(vec4(-1, 1, 1, 1));
	//l2.color = vec4(0.1f, 0.1f, 1.0f, 0);
	obj.diffuse = "SoulSpearsoulspear_diffuse.tga";					// default
	obj.mesh = "SoulSpear_Low:SoulSpear_Low1";						// default
	obj.tris = "SoulSpear_Low:SoulSpear_Low1";						// default

	plane.diffuse = "Stone";
	plane.mesh = "Quad";
	plane.tris = "Quad";

	plane.transform = glm::scale(vec3(5, 5, 5));
	plane.transform = glm::rotate(plane.transform, 270.0f, vec3(1, 0, 0));

	sp.shader = "ShadowShader";
	sp.fbo = "ShadowFBO";

	fp.shader = "Light";
	fp.fbo = "Forward";
	fp.shadowMap = "ShadowDepth";

	//pp.shader = "ParticleShader";
	pp.shader = "GPUParticleShader";
	pp.fbo = "ParticlesFBO";

	cp.shader = "Post";
}



void TestApp::onStep()
{
 	auto &ass = nsfw::Assets::instance();

	camera.Input(10.0f, nsfw::Window::instance().getDeltaTime());
	float dt = nsfw::Window::instance().getDeltaTime();
	obj.transform = glm::rotate(obj.transform, (dt * 10), vec3(0, 1, 0));

	sp.prep();
	sp.draw(light, obj);
	sp.draw(light, plane);
	sp.post();

	fp.prep();
	fp.draw(camera, obj, light);
	fp.draw(camera, plane, light);
	fp.post();

	pp.prep();
	//pp.draw(m_emitter, camera);
	pp.GPUdraw(m_GPUemitter, camera);
	pp.post();

	cp.prep();
	cp.draw();
	cp.post();
}

void TestApp::onTerm()
{

}