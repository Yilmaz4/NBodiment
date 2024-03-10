#version 430 core

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform ivec2 screenSize;

void main() {
	fragColor = texture(screenTexture, vec2(gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y));
}