#version 410

in vec2 vTexCoord;
in vec4 vNormal;
in vec4 vPosition;

out vec4 FragColor;

uniform sampler2D Diffuse;
uniform vec3 camPos;

vec3 lightColor   = vec3(0.1f, 1.0f, .1f);
vec3 lightDir     = vec3( -1 , -0.5f, 0);
float specPower   = 25.0f;

vec3 ambientColor = vec3( 0.2f, 0.2f, 0.2f); 
// need to pass in from vertex shader 

void main()
{
	vec3 colorMap = texture(Diffuse, vTexCoord).xyz;

	// AMBIENT = initial color
	vec3 ambient  = colorMap * ambientColor;

	// DIFFUSE uses Lambert's Cosine Law = dot product between normals and lightDir
	// - make light negative to make vectors same direction
	// - make dot product only positive
	// - multiply the light

	float lambert = max(0.0f, dot(vNormal.xyz, -lightDir));
	vec3 l_color = colorMap * lightColor * lambert;

	// SPECULAR dot product of reflected vec and vec from surface to the viewer
	vec3 reflectVec = normalize(reflect(lightDir, vNormal.xyz));
	vec3 eye  = normalize(camPos - vPosition.xyz);

	float temp = max(0.0f, dot(reflectVec, eye));
	float specTerm = pow(temp, specPower); 
	vec3 specular = ambientColor * specTerm * lightColor;

	FragColor = vec4(ambient + l_color + specular ,1);
	//FragColor = vNormal;
	//FragColor = vec4(l_color, 1.0f);
	//FragColor = vec4(specular ,1);
}