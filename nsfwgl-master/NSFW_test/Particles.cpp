#include "Particles.h"

GPUParticleEmitter::GPUParticleEmitter()
{
	m_particles = NULL;
	m_maxParticles = 0;
	m_position = vec3(0, 0, 0);
	m_drawShader = 0;
	m_updateShader = 0;
	m_lastDrawTime = 0;
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	// delete shaders
	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUParticleEmitter::Initialize(unsigned int a_maxParticles,
	float a_lifetimeMin, float a_lifetimeMax,
	float a_velocityMin, float a_velocityMax,
	float a_startSize, float a_endSize,
	const vec4 &a_startColor, const vec4 &a_endColor)
{
	m_startColor	= a_startColor;
	m_endColor		= a_endColor;
	m_startSize		= a_startSize;
	m_endSize		= a_endSize;
	m_velocityMin	= a_velocityMin;
	m_velocityMax	= a_velocityMax;
	m_lifespanMin	= a_velocityMin;
	m_lifespanMax	= a_velocityMax;
	m_maxParticles	= a_maxParticles;

	m_particles = new GPUParticle[a_maxParticles];

	m_activeBuffer = 0;

	createBuffers();
	createUpdateShader();
	createDrawShader();
}

// change to be drawn in ParticlePass
void GPUParticleEmitter::Draw(float time, const mat4& a_cameraTransform, const mat4& a_projectionView)
{
	// update particles using trtansform feedback
	glUseProgram(m_updateShader);

	// bind time information
	int loc = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(loc, time);

	float deltaTime = time - m_lastDrawTime;
	m_lastDrawTime = time;

	loc = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(loc, deltaTime);

	// bind emiter's position
	loc = glGetUniformLocation(m_updateShader, "emitterPosition");

	glUniform3fv(loc, 1, &m_position[0]);

	glEnable(GL_RASTERIZER_DISCARD);

	// bind buffer 
	glBindVertexArray(m_vao[m_activeBuffer]);

	// set other bufffer
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	//bidn other buffer to update as points and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	//disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	// draw particles using geo shader to billboard
	glUseProgram(m_drawShader);

	loc = glGetUniformLocation(m_drawShader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, false, &a_projectionView[0][0]);

	loc = glGetUniformLocation(m_drawShader, "CameraTransform");
	glUniformMatrix4fv(loc, 1, false, &a_cameraTransform[0][0]);

	// draw particles in other buffer
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// swap for next frame
	m_activeBuffer = otherBuffer;
}

void GPUParticleEmitter::createBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	// fisrt buffer
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// velocity
	glEnableVertexAttribArray(2);	// lifetime
	glEnableVertexAttribArray(3);	// lifespan
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	// second buffer
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// velocity
	glEnableVertexAttribArray(2);	// lifetime
	glEnableVertexAttribArray(3);	// lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// load update particle buffers
void GPUParticleEmitter::createUpdateShader()
{
	// ********* i have a load shader in TestApp also ****************//
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "../resources/shaders/gpuParticleUpdate.vert");
	
	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);

	//specify data
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_updateShader);

	// remove uneeded handles
	glDeleteShader(vs);

	// bind shader
	glUseProgram(m_updateShader);

	// bind lifetime min and max
	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);
	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);
}

// load draw shader: create shader program handle, delete shaders, set uniforms
void GPUParticleEmitter::createDrawShader()
{
	//****** also in TestApp******************//
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "../resources/shaders/gpuParticle.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "../resources/shaders/gpuParticle.frag");
	unsigned int gs = loadShader(GL_GEOMETRY_SHADER, "../resources/shaders/gpuParticle.geom");

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glAttachShader(m_drawShader, gs);
	glLinkProgram(m_drawShader);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);

	glUseProgram(m_drawShader);

	//set uniforms
	int loc = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(loc, m_startSize);
	loc = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(loc, m_endSize);

	loc = glGetUniformLocation(m_drawShader, "colorStart");
	glUniform4fv(loc, 1, &m_startColor[0]);
	loc = glGetUniformLocation(m_drawShader, "colorEnd");
	glUniform4fv(loc, 1, &m_endColor[0]);
}

unsigned int GPUParticleEmitter::loadShader(unsigned int type, const char* path) 
{
	FILE* file = fopen(path, "rb");
	//errno_t err = fopen_s(&file, path, "rb");
	if (!file)
	{
		std::cout << "couldn't open file." << std::endl;
		return 0;
	}

	// read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* Source = new char[length + 1];
	memset(Source, 0, length + 1);
	fread(Source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &Source, 0);
	glCompileShader(shader);
	/*delete[] source;
	return shader;

	std::ifstream gin(path);
	std::string contents((std::istreambuf_iterator<char>(gin)), std::istreambuf_iterator<char>());

	const char* Source = contents.c_str();

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &Source, 0);
	glCompileShader(shader);
*/
	GLint status;
	glGetProgramiv(shader, GL_LINK_STATUS, &status);
	//if (status != GL_TRUE)	// do we have a problem?!
	//{
	//	GLsizei logLen = 0;
	//	GLchar errorMsg[1024];
	//	glGetProgramInfoLog(shader, 1024, &logLen, errorMsg);

	//	std::cerr << errorMsg << std::endl;
	//	//assert(false);
	//}

	if (status == GL_FALSE)
	{
		int length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* log = new char[length];
		glGetShaderInfoLog(shader, length, 0, log);
		std::cout << "Error compiling shader.\n" << log << std::endl;
		delete[] log;
		return 0;
	}

	delete[] Source;
	return shader;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParticleEmitter::ParticleEmitter()
{
	m_firstDead = 0;
	m_maxParticles = 0;
	m_position = vec3(0, 0, 0);

	m_activeBuffer = 0;
	m_vao[0] = m_vao[1], m_ibo = m_vbo[0], m_vbo[1] = 0;
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
	glGenVertexArrays(2, m_vao);
	glBindVertexArray(m_vao[0]);
	glGenBuffers(2, m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour  (changing to vel)
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);
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
			//update move, size and color
			p->pos += p->velocity * a_deltaTime;
			p->size = glm::mix(m_startSize, m_endSize, p->lifetime / p->lifespan);
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

void ParticleEmitter::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);

	glBindVertexArray(m_vao[0]);
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

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);
	glDeleteBuffers(1, &m_ibo);
}