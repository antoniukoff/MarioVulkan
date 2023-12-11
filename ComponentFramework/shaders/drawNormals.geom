#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles) in; /// bringing in triangles

layout (line_strip, max_vertices = 2) out; 

layout (std140, binding = 0) uniform CameraMatricies {
    mat4 view;
    mat4 projection;
};


layout (location = 0) in VertexStage {
    vec3 normal;
} vs_in[];

layout (binding = 3) uniform NormalUBO {
	float len;
    float r, g, b;
} normalUBO;

void main() {
    float length  = normalUBO.len;
    for(int index = 0; index < 3; index++){
        gl_Position = projection * gl_in[index].gl_Position;
        EmitVertex();

        gl_Position = projection * (gl_in[index].gl_Position + (vec4(vs_in[index].normal, 1.0) * length));
        EmitVertex();

        EndPrimitive();
    }
}