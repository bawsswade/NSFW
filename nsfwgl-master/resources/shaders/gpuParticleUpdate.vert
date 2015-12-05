#version 410

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Velocity;
layout(location = 2) in float Lifetime;
layout(location = 3) in float Lifespan;

out vec3 v_position;
out vec3 v_velocity;
out float f_lifetime;
out float f_lifespan;

uniform float time;
uniform float deltaTime;
uniform float lifeMin;
uniform float lifeMax;
uniform vec3 emitterPostion;

const float INVERSE_MAX_UNIT = 1.0f / 4294967295.0f;

float rand(uint seed, float range){
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i <<5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UNIT;
}

void main()
{
	v_position = Position + Velocity * deltaTime;
	v_velocity = Velocity;
	f_lifetime = Lifetime;
	f_lifespan = Lifespan;
	
	if (f_lifetime > f_lifespan)
	{
		uint seed = uint(time * 1000.0) + uint(gl_VertexID);
		v_velocity.x = rand(seed++, 2) - 1;
		v_velocity.y = rand(seed++, 2) - 1;
		v_velocity.z = rand(seed++, 2) - 1;
		v_velocity = normalize(v_velocity);
		v_position = emitterPostion;
		f_lifetime = 0;
		f_lifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;
	}
}