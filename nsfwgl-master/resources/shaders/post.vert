#version 410
layout(location = 0) in vec4 Position;
layout(location = 3) in vec2 TexCoords;

out vec2 vTexCoords;



void main()
{
	vTexCoords = TexCoords;
	gl_Position = Position;
}