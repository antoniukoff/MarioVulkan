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

struct Light {
    vec4 position;
    vec4 diffuse;
};

layout (binding = 1) uniform GlobalLighting {
    Light lights[3];
} gLights;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 lightDir[3];
layout (location = 4) out vec3 eyeDir; 
layout (location = 5) out vec2 texCoords;

void main() {
    texCoords = uvCoords;

    // Compute the normal matrix
    mat3 normalMatrix = mat3(inverse(transpose(ubo.model)));
    vertNormal = normalize(normalMatrix * vNormal.xyz);

    // Compute vertex position and direction
    vec3 vertPos = vec3(ubo.view * ubo.model * vVertex);
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;

    // Compute light directions
    for (int i = 0; i < 3; i++) {
        lightDir[i] = normalize(gLights.lights[i].position.xyz - vertPos);
    }

    gl_Position = ubo.proj * ubo.view * ubo.model * vVertex;
}
