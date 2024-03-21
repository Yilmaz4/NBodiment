#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main() {
    vec4 pos = projMatrix * mat4(mat3(viewMatrix)) * vec4(aPos, 1.0);
    gl_Position = vec4(pos.xyww);
}