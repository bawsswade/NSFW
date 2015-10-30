#version 410

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D Diffuse;

void main()
{
	FragColor = texture(Diffuse, vTexCoords);
}