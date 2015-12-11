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
out vec4 vShadowCoord;
out float fTime;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 LightMatrix;
uniform mat4 LightOffset;
uniform float time;

void main()
{
	vNormal = normalize( Model * Normal).xyz;
	vTexCoord = TexCoord;
	vPosition = Model * Position;
	vTangent = Tangent.xyz;
	vBiTangent = cross(vNormal.xyz, vTangent.xyz);
	fTime = time;
	
	gl_Position = Projection * View * Model * Position;	// location on da screen for this vertex
	vShadowCoord = LightOffset * LightMatrix * Model * Position;
}