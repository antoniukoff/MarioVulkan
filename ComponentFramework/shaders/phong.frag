#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vertNormal;
layout (location = 1) in vec3 lightDir;
layout (location = 2) in vec3 eyeDir; 
layout (location = 3) in vec2 texCoords;

layout (binding = 1) uniform GlobalLighting{
	vec4 lightPos;
	vec4 diffuse;
} gLights;

layout (binding = 2 ) uniform sampler2D myTexture;

layout(location = 0) out vec4 fragColor;

void main() { 
	vec4 ks = gLights.diffuse;
	vec4 kTexture = texture(myTexture,texCoords);
	vec4 ka = vec4(0.2,0.2,0.2,1.0) * kTexture;

	float diff = max(dot(vertNormal, lightDir), 0.0);
	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source
	vec3 reflection = normalize(reflect(-lightDir, vertNormal));
	float spec = max(dot(eyeDir, reflection), 0.0);
	if(diff > 0.0){
		spec = pow(spec,14.0);
	}
	fragColor =  ka + (diff * kTexture * gLights.diffuse) + (spec * ks);	
} 

