#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout (binding = 3) uniform NormalUBO {
	float len;
    float r, g, b;
} normalUBO;

void main() {
    fragColor = vec4(normalUBO.r, normalUBO.g, normalUBO.b, 1.0);
}