#version 430 core

out vec4 fragColor;
in vec3 normal;

uniform vec3 ambientLight;

void main() {
    vec3 c = vec3(0.8) * ambientLight * normal;
    fragColor = vec4(c, 1.0);
}