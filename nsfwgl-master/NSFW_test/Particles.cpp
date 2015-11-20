#include "Particles.h"

ParticleEmitter::ParticleEmitter()
{
	m_firstDead = 0;
	m_maxParticles = 0;
	m_position = vec3(0, 0, 0);
	
	m_vao = m_ibo = m_vbo = 0;
	m_vertexData = NULL;
}

void ParticleEmitter::Create(unsigned int a_maxParticles, unsigned int a_emitRate, 
	float a_lifetimeMin, float a_lifetimeMax,
	float a_velocityMin, float a_velocityMax,
	float a_startSize, float a_endSize,
	const vec4 &a_startColor, const vec4 &a_endColor)
{
	// set up emit timers
	m_emitTimer = 0;
	m_emitRate = 1.0f / a_emitRate;

	// store all variables passed in
	m_startColor = a_startColor;
	m_endColor = a_endColor;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;
	m_lifespanMin = a_lifetimeMin;
	m_lifespanMax = a_lifetimeMax;
	m_maxParticles = a_maxParticles;



	// create particle array
	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;

	// 4 vertices per particle for a quad
	m_vertexData = new ParticleVertex[m_maxParticles * 4];
	// 6 indices per quad of 2 triangles
	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	for (unsigned int i = 0; i < m_maxParticles; ++i) {
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;
		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	// create opengl buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 *
		sizeof(ParticleVertex), m_vertexData,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 *
		sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] indexData;
}

void ParticleEmitter::Update(float a_deltaTime, mat4 &a_cameraTransform)
{
	// update timer and compare to emit rate
	m_emitTimer += a_deltaTime;
	while (m_emitTimer > m_emitRate) {
		Emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;
	// update all particle's lifespan, position, size, color
	for (unsigned int i = 0; i < m_firstDead; i++)
	{
		Particle *p = &m_particles[i];
		p->lifetime += a_deltaTime;
		// if expired, swap with firstDead
		if (p->lifetime >= p->lifespan)
		{
			*p = m_particles[m_firstDead - 1];
			m_firstDead--;
		}
		else
		{
			//move particle
			p->pos += p->velocity * a_deltaTime;
			// size particle
			p->size = glm::mix(m_startSize, m_endSize, p->lifetime / p->lifespan);
			// colour particle
			p->color = glm::mix(m_startColor, m_endColor, p->lifetime / p->lifespan);
		}

		// update vao for particles, setting up billboarded quads (optional for now)
		// update size, pos, vertices
		
		float halfSize = p->size * 0.8f;
		m_vertexData[quad * 4 + 0].position = vec4(halfSize, halfSize, 0, 1);
		m_vertexData[quad * 4 + 0].color = p->color;

		m_vertexData[quad * 4 + 1].position = vec4(-halfSize, halfSize, 0, 1);
		m_vertexData[quad * 4 + 1].color = p->color;

		m_vertexData[quad * 4 + 2].position = vec4(-halfSize, -halfSize, 0, 1);
		m_vertexData[quad * 4 + 2].color = p->color;

		m_vertexData[quad * 4 + 3].position = vec4(halfSize, -halfSize, 0, 1);
		m_vertexData[quad * 4 + 3].color = p->color;		vec3 zAxis = normalize(vec3(a_cameraTransform[3]) - p->pos);
		vec3 xAxis = cross(vec3(a_cameraTransform[1]), zAxis);
		vec3 yAxis = cross(zAxis, xAxis);
		mat4 billboard(	vec4(xAxis, 0),
						vec4(yAxis, 0),
						vec4(zAxis, 0),
						vec4(0, 0, 0, 1));		m_vertexData[quad * 4 + 0].position = billboard * m_vertexData[quad * 4 + 0].position +	vec4(p->pos, 0);
		m_vertexData[quad * 4 + 1].position = billboard * m_vertexData[quad * 4 + 1].position +	vec4(p->pos, 0);
		m_vertexData[quad * 4 + 2].position = billboard * m_vertexData[quad * 4 + 2].position +	vec4(p->pos, 0);
		m_vertexData[quad * 4 + 3].position = billboard * m_vertexData[quad * 4 + 3].position +	vec4(p->pos, 0);

		quad++;
	}	
}

/*	// sync the particle vertex buffer
	// based on how many alive particles there are
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);
	
	// draw particles
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);*/

void ParticleEmitter::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);
}

void ParticleEmitter::Emit()
{
	// make sure there is a dead particle to use
	if (m_firstDead >= m_maxParticles) return;

	// use 1st dead particle
	Particle& p = m_particles[m_firstDead++];
	// set at start position
	p.pos = m_position;

	// rand lifespan
	p.lifespan = 0;
	p.lifetime = 0;
	p.lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;
	// set start size and color
	p.color = m_startColor;
	p.size = m_startSize;

	// rand vel, dir, and strength
	float velocity = (rand() / (float)RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;
	p.velocity.x   = (rand() / (float)RAND_MAX) * 2 - 1;
	p.velocity.y   = (rand() / (float)RAND_MAX) * 2 - 1;
	p.velocity.z   = (rand() / (float)RAND_MAX) * 2 - 1;
	p.velocity = glm::normalize(p.velocity) * velocity;
}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}