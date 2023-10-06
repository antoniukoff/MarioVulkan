#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in  vec4 vVertex;
layout (location = 1) in  vec4 vNormal;
layout (location = 2) in  vec2 uvCoords;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 1) uniform GlobalLighting{
	vec4 lightPos;
	vec4 diffuse;
} gLights;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 lightDir;
layout (location = 2) out vec3 eyeDir; 
layout (location = 3) out vec2 texCoords;


/// how does the alignment work when passing from the cpu to gpu
void main() {
	texCoords = uvCoords;
	/// why do we make a normal martrix
	mat3 normalMatrix = mat3(inverse(transpose(ubo.model)));
	vertNormal = normalize(normalMatrix * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vec3 vertPos = vec3(ubo.view * ubo.model * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	lightDir = normalize(gLights.lightPos.xyz - vertPos); /// Create the light direction.
	
	gl_Position =  ubo.proj * ubo.view * ubo.model * vVertex; 
}
