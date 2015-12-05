#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "nsfw.h"
#include "Assets.h"
#include "Camera.h"

struct GPUParticle {
	//GPUPartice() : lifetime(1), lifespan(0) {}
	vec3 position;
	vec3 velocity;
	float lifetime = 1;
	float lifespan = 0;
};

class GPUParticleEmitter {
public:
	GPUParticleEmitter();
	virtual ~GPUParticleEmitter();

	void Initialize(unsigned int a_maxParticles,
		float a_lifetimeMin, float a_lifetimeMax,
		float a_velocityMin, float a_velocityMax,
		float a_startSize, float a_endSize,
		const vec4 &a_startColor, const vec4 &a_endColor);

	void Draw(float time, const mat4& a_cameraTransform, const mat4& a_projectionView);

private:
	void createBuffers();

	// load update particle buffers
	void createUpdateShader();

	// load draw shader: create shader program handle, delete shaders, set uniforms
	void createDrawShader();

	unsigned int loadShader(unsigned int type, const char* path);

	GPUParticle*	m_particles;

	unsigned int	m_maxParticles;
	vec3			m_position;
	
	float			m_lifespanMin;
	float			m_lifespanMax;

	float			m_velocityMin;
	float			m_velocityMax;

	float			m_startSize;
	float			m_endSize;

	vec4			m_startColor;
	vec4			m_endColor;

	unsigned int	m_activeBuffer;
	unsigned int	m_vao[2];
	unsigned int	m_vbo[2];

	// shader handles
	unsigned int	m_drawShader;
	unsigned int	m_updateShader;

	float		m_lastDrawTime;
};


struct Particle {
	vec3 pos;
	vec3 velocity;
	vec4 color;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex {
	vec4 position;
	vec4 color;
};

// emit particles, update, and draw
class ParticleEmitter {
public: 
	ParticleEmitter();

	void Create(unsigned int a_maxParticles, unsigned int a_emitRate,
		float a_lifetimeMin, float a_lifetimeMax,
		float a_velocityMin, float a_velocityMax,
		float a_startSize, float a_endSize,
		const vec4 &a_startColor, const vec4 &a_endColor);
	void Update(float a_deltaTime, mat4 &a_cameraTransform);
	void Draw();

	virtual ~ParticleEmitter();

public:
	Particle*		m_particles;
	vec3			m_position;
	
	unsigned int	m_firstDead;
	unsigned int	m_maxParticles;

	float			m_emitTimer;
	float			m_emitRate;

	float			m_velocityMin;
	float			m_velocityMax;

	float			m_startSize;
	float			m_endSize;

	float			m_lifespanMin;
	float			m_lifespanMax;

	vec4			m_startColor;
	vec4			m_endColor;

	ParticleVertex*	m_vertexData;

	unsigned int m_activeBuffer;
	unsigned int m_vao[2], m_vbo[2], m_ibo;

	bool Comparison(Particle p1, Particle p2)
	{
		return p1.lifetime > p2.lifetime;
	}

	void Emit();
	
};