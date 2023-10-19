#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 vertNormal;
layout (location = 1) in vec3 lightDir[3];
layout (location = 4) in vec3 eyeDir; 
layout (location = 5) in vec2 texCoords;

struct Light {
    vec4 position;
    vec4 diffuse;
};

layout (binding = 1) uniform GlobalLighting {
    Light lights[2];
} gLights;

layout (binding = 2) uniform sampler2D myTexture;

layout(location = 0) out vec4 fragColor;

void main() { 
	vec4 colorOutput = vec4(0, 0, 0, 1);
	for(int i = 0; i < 3; i++){

	 vec4 ks = gLights.lights[i].diffuse;
	vec4 kTexture = texture(myTexture,texCoords);
	vec4 ka = vec4(0.2,0.2,0.2,1.0) * kTexture;

	float diff = max(dot(vertNormal, lightDir[i]), 0.0);
	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source
	vec3 reflection = normalize(reflect(-lightDir[i], vertNormal));
	float spec = max(dot(eyeDir, reflection), 0.0);
	if(diff > 0.0){
		spec = pow(spec,14.0);
	}
	colorOutput += ka + (diff * kTexture * gLights.lights[i].diffuse) + (spec * ks);
	}
	fragColor =  colorOutput;	
} 