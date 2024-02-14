#version 430 core

layout(std430, binding = 0) volatile buffer vBuffer {
    vec4 vertices[];
};

uniform double uTimeDelta;

uniform mat4 uMatrix;

out vec3 vertexColor;

void main() {
    int i = gl_VertexID * 2;
    vec4 vel = vec4(vertices[i + 1].xy * float(uTimeDelta), 0, 0);
    vertices[i] += vel;
    gl_Position = uMatrix * vertices[i];
    vertexColor = vec3(1, 1, 1);
}