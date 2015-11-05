#version 410

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D Final;
uniform sampler2D Depth;

void main()
{
		FragColor = texture(Final, vTexCoords);
		if(vTexCoords.x < 0.5f)
			FragColor = texture(Depth, vTexCoords);
			
		//FragColor = vec4(1,1,0,1);
}