#pragma once
#include "nsfw.h"
#include "Assets.h"
#include "Camera.h"

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

	unsigned int m_vao, m_vbo, m_ibo;

	bool Comparison(Particle p1, Particle p2)
	{
		return p1.lifetime > p2.lifetime;
	}

	void Emit();
	
};