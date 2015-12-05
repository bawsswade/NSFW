#version 410

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D Final;
uniform sampler2D Depth;

uniform sampler2D Particles;
uniform sampler2D ParticleDepth;

void main()
{
		vec3 final = texture(Final, vTexCoords).rgb;
		vec3 particles = texture(Particles, vTexCoords).rgb;
		
		vec3 gd = texture(Depth, vTexCoords).rgb;
		vec3 pd = texture(ParticleDepth, vTexCoords).rgb;
		
		//if(vTexCoords.x < 0.5f)
			//FragColor = texture(Depth, vTexCoords);
		FragColor = vec4( final + particles, 1);
		
		// for CPU particles
		//if(gd.z < pd.z)
		//FragColor = vec4(final,1);
		//else FragColor = vec4(particles,1);
		
		FragColor = vec4(particles, 1);
}