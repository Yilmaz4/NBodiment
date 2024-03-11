#version 430 core

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform ivec2 screenSize;
uniform float transparency;

void main() {
	fragColor = vec4(texture(screenTexture, vec2(gl_FragCoord.x / screenSize.x, gl_FragCoord.y / screenSize.y)).rgb, transparency);
}