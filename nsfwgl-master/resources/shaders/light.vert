#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec4 Tangent;
layout(location=3) in vec2 TexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vPosition;
out vec3 vTangent;
out vec3 vBiTangent;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	vNormal = normalize(View * Model * Normal).xyz;
	vTexCoord = TexCoord;
	vPosition = Model * Position;
	vTangent = Tangent.xyz;
	vBiTangent = cross(vNormal.xyz, vTangent.xyz);

	gl_Position = Projection * View * Model * Position;	// location on da screen for this vertex
}