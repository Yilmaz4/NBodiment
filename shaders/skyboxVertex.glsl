#version 430 core
layout(location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main() {
    texCoords = aPos;
    vec4 pos = projMatrix * viewMatrix * vec4(aPos, 1.0);
    gl_Position = vec4(pos.xyww);
}