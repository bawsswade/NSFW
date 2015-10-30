#version 410
layout(location = 0) in vec4 Position;
layout(location = 3) in vec2 TexCoords;

out vec2 vTexCoords;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	vTexCoords = TexCoords;
	gl_Position = Projection * View * Model * Position;
}